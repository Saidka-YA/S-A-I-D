#include "xor.h"

// Ключ из ключевого слова
vector<int> xorKeyFromWord(const string& word, const vector<char32_t>& alphabet) {
    vector<char32_t> codes = to_codes(word);
    vector<int> key;
    for (char32_t c : codes) {
        bool found = false;
        for (int i = 0; i < (int)alphabet.size(); i++) {
            if (alphabet[i] == c) {
                key.push_back(i);
                found = true;
                break;
            }
        }
        if (!found) {
            cerr << "Ошибка: символ U+" << hex << uppercase << (uint32_t)c << " не найден в алфавите!\n";
            return {};
        }
    }
    return key;
}

// Шифрование дешифрование
string xor_encrypt(const string& text, const string& key, const vector<char32_t>& alphabet) {
    vector<char32_t> idx = to_codes(text);
    vector<int> keyidx = xorKeyFromWord(key, alphabet);
    if (keyidx.empty()) return "";
    
    int keylen = keyidx.size();
    int m = alphabet.size();
    vector<char32_t> residx;

    for (int i = 0; i < idx.size(); ++i) {
        int textIdx = -1;
        for (int j = 0; j < alphabet.size(); ++j) {
            if (alphabet[j] == idx[i]) { 
                textIdx = j;
                break;
            }
        }
        if (textIdx == -1) {
            cerr << "Ошибка: символ U+" << hex << uppercase << (uint32_t)idx[i] << " не найден в алфавите!\n";
            return "";
        }
        
        // xor индексов по модулю размера алфавита
        int indx = (textIdx ^ keyidx[i % keylen]) % m;
        residx.push_back(alphabet[indx]);
    }
    return to_text(residx);
}

string xor_decrypt(const string& text, const string& key, const vector<char32_t>& alphabet) {
    return xor_encrypt(text, key, alphabet);
}