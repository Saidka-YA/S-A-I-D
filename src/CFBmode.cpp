#include "CFBmode.h"
#include "utilities.h"
#include "AES128.h"
using namespace std;
using namespace aes;

void encryptCFB(const vector<uint8_t>& plaintext, vector<uint8_t>& ciphertext, 
                uint8_t expandedKey[176])
{
    // Генерируем IV
    uint8_t iv[16];
    generateIV(iv);
    // Записываем IV в начало сiphertext
    for(int i = 0; i < 16; i++)
    {
        ciphertext.push_back(iv[i]);
    }
    // Инициализация и заполнение prev
    // prev это предыдущий блок ciphertext
    // Начальный prev это вектор инициализации IV 
    uint8_t prev[16];
    memcpy(prev, iv, 16); // Копируем 16 байт iv в prev

    size_t size = plaintext.size();
    // Ходим блоками по 16 байт
    for(size_t i = 0; i < size; i += 16)
    {
        // Шифруем prev
        uint8_t encPrev[16]; // Сюда запишем зашифрованный prev
        encryptBlock(prev, encPrev, expandedKey); // Шифруем

        // "Магия" CFB мы XORим байты с входных данных с зашифрованным prev 
        // Это делает из блочного шифрования практически потоковое шифрование
        uint8_t block[16];
        for(int j = 0; j < 16; j++)
        {
            block[j] = plaintext[i+j] ^ encPrev[j];
        }

        for(int j = 0; j < 16; j++)
        {
            ciphertext.push_back(block[j]); // Записываем в ciphertext готовый блок
            prev[j] = block[j]; // текущий ciphertext становится prev 
        }
    }
}

void decryptCFB(const vector<uint8_t>& ciphertext, vector<uint8_t>& plaintext, 
                uint8_t expandedKey[176])
{
    // первые 16 байт это IV
    if(ciphertext.size() < 32) return;
    // Копируем первые 16 байт в prev
    uint8_t prev[16];
    memcpy(prev, &ciphertext[0], 16);

    size_t size = ciphertext.size();
    // Ходим блоками по 16 байт
    for(size_t i = 16; i < size; i += 16)
    {
        // Защита от выхода за границу массива
        if(i + 16 > size) break;

        uint8_t encPrev[16];
        encryptBlock(prev, encPrev, expandedKey); // Получаем ключи для дальнейшей расшифровки

        for(int j = 0; j < 16; j++)
        {
            // "Магия" CFB мы XORим ciphertext с AES(prev) и получаем plaintext
            plaintext.push_back(ciphertext[i+j] ^ encPrev[j]);
        }
        // Текущий cyphertext становится prev 
        memcpy(prev, &ciphertext[i], 16);
    }
}