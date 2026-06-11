#include "AES128.h"
using namespace std;

// Функция умножения в поле Галуа
uint8_t gmul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    for (int i = 0; i < 8; i++)
    {
        if (b & 1) p ^= a;
        uint8_t hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

// Функция добавления раундового ключа
void addRoundKey(uint8_t state[4][4], uint8_t expandedKey[176], int round)
{
    int start = round * 16; // Вычисляем начало раундового ключа
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            state[row][column] ^= expandedKey[start + column*4 + row]; // Добавляем раундовый ключ
        }
    }
}

// Функция для замены байтов по таблице S-box
void subBytes(uint8_t state[4][4])
{
    for (int r = 0; r < 4; r++)
    {        
        for (int c = 0; c < 4; c++)
        {    
            state[r][c] = sbox[state[r][c]]; // подстановка байтов
        }
    }
}

// Сдвиг строк 
void shiftRows(uint8_t state[4][4])
{
    uint8_t temp;

    temp=state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3]; 
    state[1][3] = temp;

    temp = state[2][0]; 
    state[2][0] = state[2][2]; 
    state[2][2] = temp;
    temp = state[2][1]; 
    state[2][1] = state[2][3]; 
    state[2][3] = temp;

    temp=state[3][3];
    state[3][3] = state[3][2]; 
    state[3][2] = state[3][1];
    state[3][1] = state[3][0]; 
    state[3][0] = temp;
}

// Операция MixColumns с умножением в поле Галуа
void mixColumns(uint8_t state[4][4])
{
    for (int c = 0; c < 4; c++)
    {
        uint8_t s0 = state[0][c];
        uint8_t s1 = state[1][c];
        uint8_t s2 = state[2][c];
        uint8_t s3 = state[3][c];

        state[0][c] = gmul(2, s0) ^ gmul(3, s1)^ s2 ^ s3;
        state[1][c] = s0 ^ gmul(2, s1) ^ gmul(3, s2) ^ s3;
        state[2][c] = s0 ^ s1^ gmul(2, s2) ^ gmul(3, s3);
        state[3][c] = gmul(3, s0)^ s1 ^ s2 ^gmul(2, s3);
    }
}

void expandRoundKey(uint8_t key[16], uint8_t expandedKey[176])
{
    memcpy(expandedKey, key, 16); // Копируем исходный ключ, это первые 16 байт
    int bytes = 16;
    int rcon = 1;
    uint8_t temp[4];
    // Генеруем пока не будет 176 байт, так как 44 слова * 4 байта = 176
    while(bytes < 176)
    {
        memcpy(temp, &expandedKey[bytes-4], 4); // Сохраняем полследнее созданное слово
        // Каждые 16 байт новый раунд
        if(bytes % 16 == 0)
        {
            // Сдвиг строк
            uint8_t t = temp[0];
            temp[0] = temp[1]; 
            temp[1] = temp[2];
            temp[2] = temp[3]; 
            temp[3] = t;
            // Замена байтов байтами из Sbox
            for(int i = 0; i < 4; i++) 
            {
                temp[i] = sbox[temp[i]];
            }
            temp[0] ^= Rcon[rcon++];
        }
        // Генерация нового слова
        for(int i=0;i<4;i++)
        {
            expandedKey[bytes]=expandedKey[bytes-16]^temp[i];
            bytes++;
        }
    }
}

// Функция шифрования
void encryptBlock(const uint8_t input[16], uint8_t output[16], uint8_t expandedKey[176])
{
    // Инициализация матрицы и её заполнение
    uint8_t state[4][4];
    for(int column = 0; column < 4; column++)
    {
        {
            for(int row = 0; row < 4; row++)
            //Принцип Column-major
            // Заполняем state по столбцам
            state[row][column] = input[column*  4 + row]; 
        }
    }
    // Начальный раундовый ключ
    addRoundKey(state, expandedKey, 0);
    // Раунды с 1 по 9
    for(int round = 1; round < 10; round++)
    {
        subBytes(state); // Замена байтов по таблице S-box
        shiftRows(state); // Сдвиг строк
        mixColumns(state); // Перемешивание столбцов
        addRoundKey(state, expandedKey, round); // Добавление раундового ключа
    }
    // Раунд 10 без перемешивания столбцов
    subBytes(state); // Замена байтов по таблице S-box
    shiftRows(state); // Сдвиг строк
    addRoundKey(state, expandedKey, 10); // Добавление ключа для раунда 10

    // Преобразование state в массив вывода
    for(int c = 0; c < 4; c++)
    {   
        for(int r = 0; r < 4; r++)
        {
            output[c * 4 + r] = state[r][c];
        }
    }
}