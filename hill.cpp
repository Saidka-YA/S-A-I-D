#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

// Генерация алфавита для шифра
vector<char32_t> genalphabet() {
    vector<char32_t> alphabet;
    for (char32_t c = U'A'; c <= U'Z'; ++c) alphabet.push_back(c);
    for (char32_t c = U'a'; c <= U'z'; ++c) alphabet.push_back(c);
    for (char32_t c = U'А'; c <= U'Я'; ++c) alphabet.push_back(c);
    for (char32_t c = U'а'; c <= U'я'; ++c) alphabet.push_back(c);
    for (char32_t c = U'0'; c <= U'9'; ++c) alphabet.push_back(c);
    const char32_t punct[] = {
        U' ', U'!', U'@', U'#', U'$', U'%', U'^', U'&', U'*',
        U'(', U')', U'-', U'_', U'=', U'+', U'\\', U'|', U'/',
        U'"', U':', U';', U'.', U',', U'?'
    };
    for (char32_t c : punct) alphabet.push_back(c);

    return alphabet;
}



int main() {
    setlocale(LC_ALL, "Rus");
    
    return 0;
}