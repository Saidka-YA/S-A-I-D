#include "vernam.h"

vector<int> vernamKeyFromText(const string& text, const vector<char32_t>& alphabet) {
    int m = to_codes(text).size();
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(0, (int)alphabet.size() - 1);
    vector<int> key;
    for (size_t i = 0; i < (size_t)m; ++i) key.push_back(dist(rng));
    return key;
}

// Шифрвока через Вернама по модулю m
string vernamEncrypt(const string& text, const vector<int>& key, const vector<char32_t>& alphabet) {
    vector<char32_t> idx = to_codes(text);
    // проверка если ключ меньше текста
    if (idx.size() > key.size()) {
        cerr << "Ошибка: ключ меньше текста!";
        return "";
    }
    int m = alphabet.size();
    vector<char32_t> result;
    for (int i = 0; i < (int)idx.size(); ++i) {
        // Ищем индекс символа в алфавите
        int textIdx = -1;
        for (int j = 0; j < (int)alphabet.size(); ++j) {
            if (alphabet[j] == idx[i]) {
                textIdx = j;
                break;
            }
        }
        if (textIdx == -1) {
            cerr << "Ошибка: символ U+" << hex << uppercase << (uint32_t)idx[i] << " не найден в алфавите!\n";
            return "";
        }
        int indx = (textIdx + key[i]) % m;
        result.push_back(alphabet[indx]);
    }
    return to_text(result);
}

// Расшифровка
string vernamDecrypt(const string& ctext, const vector<int>& key, const vector<char32_t>& alphabet) {
    vector<char32_t> idx = to_codes(ctext);
    if (idx.size() > key.size()) {
        cerr << "Ошибка: ключ меньше текста!";
        return "";
    }
    int m = alphabet.size();
    vector<char32_t> result;
    for (int i = 0; i < (int)idx.size(); ++i) {
        // Ищем индекс символа в алфавите
        int textIdx = -1;
        for (int j = 0; j < (int)alphabet.size(); ++j) {
            if (alphabet[j] == idx[i]) {
                textIdx = j;
                break;
            }
        }
        if (textIdx == -1) {
            cerr << "Ошибка: символ U+" << hex << uppercase << (uint32_t)idx[i] << " не найден в алфавите!\n";
            return "";
        }
        int indx = (textIdx - key[i] + m) % m;
        result.push_back(alphabet[indx]);
    }
    return to_text(result);
}