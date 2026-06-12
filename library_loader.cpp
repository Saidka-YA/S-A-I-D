#include "library_loader.h"
#include <iostream>
#include <string>

using namespace std;

#if defined(_WIN32) || defined(_WIN64)
    #define LIB_PREFIX "lib"
    #define LIB_EXT ".dll"
    #define LOAD_LIB(name) LoadLibraryA(name)
    #define GET_FUNC(h, name) GetProcAddress(h, name)
    #define CLOSE_LIB(h) FreeLibrary(h)
#else
    #define LIB_PREFIX "lib"
    #define LIB_EXT ".so"
    #define LOAD_LIB(name) dlopen(name, RTLD_NOW)
    #define GET_FUNC(h, name) dlsym(h, name)
    #define CLOSE_LIB(h) dlclose(h)
#endif

LibraryLoader::LibraryLoader(const string& algo_name) {
    string path = "./" + string(LIB_PREFIX) + algo_name + LIB_EXT;
    handle_ = LOAD_LIB(path.c_str());
    if (!handle_) {
        throw runtime_error("Не удалось загрузить библиотеку алгоритма: " + algo_name);
    }

    info_fn_ = (GetAlgoInfoFunc)GET_FUNC(handle_, "get_algorithm_info");
    size_fn_ = (GetOutputSizeFunc)GET_FUNC(handle_, "get_output_size");
    crypto_fn_ = (CryptoFunc)GET_FUNC(handle_, "encrypt"); 
    validate_fn_ = (ValidateKeyFunc)GET_FUNC(handle_, "validate_key");

    if (!info_fn_ || !size_fn_ || !crypto_fn_ || !validate_fn_) {
        CLOSE_LIB(handle_);
        throw runtime_error("Библиотека не соответствует унифицированному интерфейсу.");
    }
}

LibraryLoader::~LibraryLoader() {
    if (handle_) CLOSE_LIB(handle_);
}

const AlgorithmInfo* LibraryLoader::get_info() const { return info_fn_(); }

size_t LibraryLoader::get_output_size(size_t input_size, bool is_encrypt) const {
    return size_fn_(input_size, is_encrypt ? 1 : 2);
}

int LibraryLoader::execute_crypto(bool is_encrypt, ConstBuffer key, ConstBuffer input, MutBuffer* output) const {
    CryptoFunc func = is_encrypt ? crypto_fn_ : (CryptoFunc)GET_FUNC(handle_, "decrypt");
    if (!func) throw runtime_error("Функция расшифрования не найдена в библиотеке.");
    return func(key, input, output);
}

int LibraryLoader::validate_key(ConstBuffer key) const {
    return validate_fn_(key);
}