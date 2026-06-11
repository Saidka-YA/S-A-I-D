#include "RSAblock.h"
#include "RSAutilities.h"
using namespace std;
using namespace rsa;

// Функция модульного возведения в степень
void modPow(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod)
{
    mpz_powm(result, base, exp, mod);
}

// Функция вычисления НОД
void gcd(mpz_t result, const mpz_t a, const mpz_t b)
{
    mpz_gcd(result, a, b);
}

// Функция вычисления обратного элемента по модулю
void modInverse(mpz_t result, const mpz_t a, const mpz_t m)
{
    mpz_invert(result, a, m);
}

// Функция генерации ключей RSA
// Параметры: n, e - открытый ключ, d - закрытый ключ
void generateKeys(mpz_t n, mpz_t e, mpz_t d)
{
    mpz_t p, q, phi, pMinus1, qMinus1, gcdResult;
    mpz_init(p);
    mpz_init(q);
    mpz_init(phi);
    mpz_init(pMinus1);
    mpz_init(qMinus1);
    mpz_init(gcdResult);
    
    // Генерируем два простых числа p и q (по 512 бит каждое для n = 1024 бит)
    generatePrime(p, 512);
    generatePrime(q, 512);
    
    // Проверяем что p != q
    while(mpz_cmp(p, q) == 0)
    {
        generatePrime(q, 512);
    }
    
    // Вычисляем n = p * q
    mpz_mul(n, p, q);
    
    // Вычисляем φ(n) = (p-1) * (q-1)
    mpz_sub_ui(pMinus1, p, 1);
    mpz_sub_ui(qMinus1, q, 1);
    mpz_mul(phi, pMinus1, qMinus1);
    
    // Выбираем e (обычно 65537)
    mpz_set_ui(e, 65537);
    
    // Проверяем что gcd(e, φ(n)) = 1
    gcd(gcdResult, e, phi);
    while(mpz_cmp_ui(gcdResult, 1) != 0)
    {
        mpz_add_ui(e, e, 2);
        gcd(gcdResult, e, phi);
    }
    
    // Вычисляем d = e^(-1) mod φ(n)
    modInverse(d, e, phi);
    
    // Очистка временных переменных
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(phi);
    mpz_clear(pMinus1);
    mpz_clear(qMinus1);
    mpz_clear(gcdResult);
}

// Функция шифрования блока
// c = m^e mod n
void encryptBlock(const uint8_t* input, int blockSize, mpz_t c, 
                  const mpz_t n, const mpz_t e)
{
    mpz_t message;
    mpz_init(message);
    
    // Преобразуем байты в mpz_t
    mpz_import(message, blockSize, 1, sizeof(uint8_t), 0, 0, input);
    
    // Проверяем что сообщение меньше n
    if(mpz_cmp(message, n) >= 0)
    {
        mpz_mod(message, message, n);
    }
    
    // Вычисляем c = m^e mod n
    modPow(c, message, e, n);
    
    mpz_clear(message);
}

// Функция расшифрования блока
// m = c^d mod n
void decryptBlock(mpz_t message, const mpz_t c, const mpz_t n, const mpz_t d)
{
    // Вычисляем m = c^d mod n
    modPow(message, c, d, n);
}

// Функция шифрования данных
void encryptData(const vector<uint8_t>& plaintext, vector<uint8_t>& ciphertext, 
                 const mpz_t n, const mpz_t e)
{
    size_t size = plaintext.size();
    int blockSize = 64; // Размер блока 64 байта
    
    mpz_t c;
    mpz_init(c);
    
    // Ходим блоками по 64 байта
    for(size_t i = 0; i < size; i += blockSize)
    {
        // Шифруем блок
        encryptBlock(&plaintext[i], blockSize, c, n, e);
        
        // Получаем размер c в байтах
        size_t cSize = (mpz_sizeinbase(c, 2) + 7) / 8;
        
        // Записываем размер c (2 байта)
        ciphertext.push_back((cSize >> 8) & 0xFF);
        ciphertext.push_back(cSize & 0xFF);
        
        // Записываем c
        vector<uint8_t> cBytes(cSize);
        mpz_export(&cBytes[0], NULL, 1, sizeof(uint8_t), 0, 0, c);
        for(size_t j = 0; j < cSize; j++)
        {
            ciphertext.push_back(cBytes[j]);
        }
    }
    
    mpz_clear(c);
}

void decryptData(const vector<uint8_t>& ciphertext, vector<uint8_t>& plaintext, 
                 const mpz_t n, const mpz_t d)
{
    size_t pos = 0;
    int blockSize = 64;
    
    mpz_t c, message;
    mpz_init(c);
    mpz_init(message);
    
    while(pos < ciphertext.size())
    {
        if(pos + 2 > ciphertext.size()) break;
        
        size_t cSize = ((size_t)ciphertext[pos] << 8) | ciphertext[pos + 1];
        pos += 2;
        
        if(pos + cSize > ciphertext.size()) break;
        
        mpz_import(c, cSize, 1, sizeof(uint8_t), 0, 0, &ciphertext[pos]);
        pos += cSize;
        
        decryptBlock(message, c, n, d);
        
        // Получаем байты числа
        size_t exportedSize = (mpz_sizeinbase(message, 2) + 7) / 8;
        vector<uint8_t> blockBytes(blockSize, 0); // Инициализируем нулями
        
        if(exportedSize <= (size_t)blockSize)
        {
            size_t count;
            mpz_export(&blockBytes[blockSize - exportedSize], &count, 1, sizeof(uint8_t), 0, 0, message);
        }
        
        for(int j = 0; j < blockSize; j++)
        {
            plaintext.push_back(blockBytes[j]);
        }
    }
    
    mpz_clear(c);
    mpz_clear(message);
}