#include "A51EncryptDecrypt.h"
#include "A51utilities.h"
#include "A51Cipher.h"
using namespace std;

// Шифрование: XOR данных с потоком гаммы
// Формат выходных данных: [4 байта IV (номер фрейма)] + [зашифрованные данные]
void encryptA51(const vector<uint8_t>& plaintext, vector<uint8_t>& ciphertext, 
                 uint8_t key[8])
{
    // Генерируем номер фрейма (IV) — 22 бита, храним в 4 байтах
    uint8_t ivBytes[4];
    generateIV(ivBytes);
    uint32_t frameNumber = ((uint32_t)ivBytes[0]) | ((uint32_t)ivBytes[1] << 8)  |
                           ((uint32_t)ivBytes[2] << 16) | ((uint32_t)ivBytes[3] << 24);
    frameNumber &= 0x3FFFFF; // Маска 22 бита

    // Записываем IV в начало ciphertext (4 байта)
    for (int i = 0; i < 4; i++)
    {
        ciphertext.push_back(ivBytes[i]);
    }

    // Инициализируем регистры A5/1
    uint32_t r1, r2, r3;
    initA51(r1, r2, r3, key, frameNumber);

    // Генерируем поток гаммы длиной равной plaintext
    vector<uint8_t> keystream;
    generateKeystream(r1, r2, r3, keystream, plaintext.size());

    // XOR plaintext с потоком гаммы
    for (size_t i = 0; i < plaintext.size(); i++)
    {
        ciphertext.push_back(plaintext[i] ^ keystream[i]);
    }
}

// Расшифрование: считываем IV, восстанавливаем поток гаммы, XOR с ciphertext
void decryptA51(const vector<uint8_t>& ciphertext, vector<uint8_t>& plaintext, 
                 uint8_t key[8])
{
    // Минимальный размер: 4 байта IV + хотя бы 1 байт данных
    if (ciphertext.size() < 5) return;

    // Считываем IV из первых 4 байт
    uint32_t frameNumber = ((uint32_t)ciphertext[0])       |
                           ((uint32_t)ciphertext[1] << 8)  |
                           ((uint32_t)ciphertext[2] << 16) |
                           ((uint32_t)ciphertext[3] << 24);
    frameNumber &= 0x3FFFFF; // Маска 22 бита

    // Инициализируем регистры A5/1 с тем же ключом и номером фрейма
    uint32_t r1, r2, r3;
    initA51(r1, r2, r3, key, frameNumber);

    // Данные начинаются с позиции 4 (после IV)
    size_t dataSize = ciphertext.size() - 4;

    // Генерируем поток гаммы длиной равной зашифрованным данным
    vector<uint8_t> keystream;
    generateKeystream(r1, r2, r3, keystream, dataSize);

    // XOR ciphertext с потоком гаммы для получения plaintext
    for (size_t i = 0; i < dataSize; i++)
    {
        plaintext.push_back(ciphertext[4 + i] ^ keystream[i]);
    }
}