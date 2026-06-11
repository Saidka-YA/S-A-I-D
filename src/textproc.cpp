#include "textproc.h"

// Генерация алфавита для шифра
vector<char32_t> genalphabet() {
    vector<char32_t> alphabet;

    // A-Z
    for (char32_t c = 0x41; c <= 0x5A; ++c) alphabet.push_back(c);
    // a-z
    for (char32_t c = 0x61; c <= 0x7A; ++c) alphabet.push_back(c);
    // А-Я + Ё
    for (char32_t c = 0x410; c <= 0x42F; ++c) alphabet.push_back(c);
    alphabet.push_back(0x401); // Ё
    // а-я + ё
    for (char32_t c = 0x430; c <= 0x44F; ++c) alphabet.push_back(c);
    alphabet.push_back(0x451); // ё
    // 0-9
    for (char32_t c = 0x30; c <= 0x39; ++c) alphabet.push_back(c);

    // Знаки препинания
    const char32_t punct[] = {
        0x20, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A,
        0x28, 0x29, 0x2D, 0x5F, 0x3D, 0x2B, 0x5C, 0x7C, 0x2F,
        0x22, 0x3A, 0x3B, 0x2E, 0x2C, 0x3F, 0xA, 0xD, 0x9,
        0xAB, 0xBB, 0x2014, 0x2013, 0x2026, 0x2018, 0x2019, 0x201C, 0x201D
    };
    for (char32_t c : punct) alphabet.push_back(c);

    return alphabet;
}

// UTF-8 строка в вектор кодпоинтов
vector<char32_t> to_codes(const string& str) {
    vector<char32_t> codes;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if (c <= 0x7F) {
            codes.push_back((char32_t)c);
            i += 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < str.length()) {
            codes.push_back((char32_t)(((str[i] & 0x1F) << 6) | (str[i+1] & 0x3F)));
            i += 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < str.length()) {
            codes.push_back((char32_t)(((str[i] & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F)));
            i += 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < str.length()) {
            codes.push_back((char32_t)(((str[i] & 0x07) << 18) | ((str[i+1] & 0x3F) << 12) | ((str[i+2] & 0x3F) << 6) | (str[i+3] & 0x3F)));
            i += 4;
        } else {
            i += 1;
        }
    }
    return codes;
}

// Вектор кодпоинтов в строку
string to_text(const vector<char32_t>& codes) {
    string result;
    for (char32_t cp : codes) {
        if (cp < 0x80) {
            result += (char)cp;
        } else if (cp < 0x800) {
            result += (char)(0xC0 | (cp >> 6));
            result += (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            result += (char)(0xE0 | (cp >> 12));
            result += (char)(0x80 | ((cp >> 6) & 0x3F));
            result += (char)(0x80 | (cp & 0x3F));
        } else {
            result += (char)(0xF0 | (cp >> 18));
            result += (char)(0x80 | ((cp >> 12) & 0x3F));
            result += (char)(0x80 | ((cp >> 6) & 0x3F));
            result += (char)(0x80 | (cp & 0x3F));
        }
    }
    return result;
}
