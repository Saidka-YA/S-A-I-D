#ifndef LIBRARY_LOADER_H
#define LIBRARY_LOADER_H

#include "crypto_interface.h"
#include <string>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    using LibHandle = HMODULE;
#else
    #include <dlfcn.h>
    using LibHandle = void*;
#endif

typedef const AlgorithmInfo* (*GetAlgoInfoFunc)();
typedef size_t (*GetOutputSizeFunc)(size_t, int);
typedef int (*CryptoFunc)(ConstBuffer, ConstBuffer, MutBuffer*);
typedef int (*ValidateKeyFunc)(ConstBuffer);

class LibraryLoader {
public:
    LibraryLoader(const std::string& algo_name);
    ~LibraryLoader();

    const AlgorithmInfo* get_info() const;
    size_t get_output_size(size_t input_size, bool is_encrypt) const;
    int execute_crypto(bool is_encrypt, ConstBuffer key, ConstBuffer input, MutBuffer* output) const;
    int validate_key(ConstBuffer key) const;

private:
    LibHandle handle_;
    GetAlgoInfoFunc info_fn_;
    GetOutputSizeFunc size_fn_;
    CryptoFunc crypto_fn_;
    ValidateKeyFunc validate_fn_;
};

#endif