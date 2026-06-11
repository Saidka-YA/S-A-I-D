#include "textproc.h"

// Прямое преобразование: каждый байт строки становится числом 0-255
vector<char32_t> to_codes(const string& str) {
    vector<char32_t> codes;
    for (unsigned char c : str) {
        codes.push_back((char32_t)c);
    }
    return codes;
}

// Обратное преобразование: берем младшие 8 бит каждого кода
string to_text(const vector<char32_t>& codes) {
    string result;
    for (char32_t cp : codes) {
        result += (char)(cp & 0xFF);
    }
    return result;
}