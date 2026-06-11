#include "vernam.h"

// генерируем случайный ключ - столько байт сколько в тексте
vector<uint8_t> vernamKeyFromText(const string& text) {
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(0, 255);
    vector<uint8_t> key;
    for (size_t i = 0; i < text.size(); ++i)
        key.push_back((uint8_t)dist(rng));
    return key;
}

// xor каждого байта текста с ключом
string vernamEncrypt(const string& text, const vector<uint8_t>& key) {
    if (text.size() > key.size()) {
        cerr << "Ошибка: ключ короче текста!\n";
        return "";
    }
    string result = text;
    for (size_t i = 0; i < text.size(); ++i)
        result[i] = text[i] ^ key[i];
    return result;
}

// дешифрование - та же операция xor
string vernamDecrypt(const string& ctext, const vector<uint8_t>& key) {
    return vernamEncrypt(ctext, key);
}
