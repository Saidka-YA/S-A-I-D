#include "A51Cipher.h"
using namespace std;

// Регистры сдвига с линейной обратной связью (РСЛОС) передаются отдельными переменными:
// r1: 19 бит (биты 0-18), r2: 22 бита (биты 0-21), r3: 23 бита (биты 0-22)
// Итого 64 бита — размер ключа

// Получение бита по позиции из 32-битного значения
uint8_t getBit(uint32_t reg, int pos)
{
    return (reg >> pos) & 1;
}

// Функция сдвига регистра R1 (19 бит)
// Полином: x^19 + x^18 + x^17 + x^14 + 1
// Биты обратной связи: 18, 17, 16, 13
void clockR1(uint32_t& r1)
{
    uint8_t feedback = getBit(r1, 18) ^ getBit(r1, 17) ^ getBit(r1, 16) ^ getBit(r1, 13);
    r1 = ((r1 << 1) | feedback) & 0x7FFFF; // Маска 19 бит
}

// Функция сдвига регистра R2 (22 бита)
// Полином: x^22 + x^21 + 1
// Биты обратной связи: 21, 20
void clockR2(uint32_t& r2)
{
    uint8_t feedback = getBit(r2, 21) ^ getBit(r2, 20);
    r2 = ((r2 << 1) | feedback) & 0x3FFFFF; // Маска 22 бита
}

// Функция сдвига регистра R3 (23 бита)
// Полином: x^23 + x^22 + x^21 + x^8 + 1
// Биты обратной связи: 22, 21, 20, 7
void clockR3(uint32_t& r3)
{
    uint8_t feedback = getBit(r3, 22) ^ getBit(r3, 21) ^ getBit(r3, 20) ^ getBit(r3,  7);
    r3 = ((r3 << 1) | feedback) & 0x7FFFFF; // Маска 23 бита
}

// Функция тактирования по принципу большинства (majority clocking)
// Тактируются только те регистры, чей средний бит совпадает с большинством
// Средние биты: r1[8], r2[10], r3[10]
void clockMajority(uint32_t& r1, uint32_t& r2, uint32_t& r3)
{
    uint8_t b1 = getBit(r1, 8);
    uint8_t b2 = getBit(r2, 10);
    uint8_t b3 = getBit(r3, 10);

    // Вычисляем бит большинства
    uint8_t majority = (b1 & b2) | (b2 & b3) | (b1 & b3);

    // Тактируем только регистры, чей бит равен биту большинства
    if (b1 == majority) clockR1(r1);
    if (b2 == majority) clockR2(r2);
    if (b3 == majority) clockR3(r3);
}

// Инициализация A5/1: загрузка 64-битного ключа и 22-битного номера фрейма
void initA51(uint32_t& r1, uint32_t& r2, uint32_t& r3, uint8_t key[8], 
             uint32_t frameNumber)
{
    // Обнуление регистров
    r1 = 0;
    r2 = 0;
    r3 = 0;

    // Загрузка 64 бит ключа (побитово, без тактирования по большинству)
    for (int i = 0; i < 64; i++)
    {
        // Тактируем все три регистра принудительно
        clockR1(r1);
        clockR2(r2);
        clockR3(r3);

        // Получаем текущий бит ключа
        uint8_t keyBit = (key[i / 8] >> (i % 8)) & 1;

        // XOR бита ключа в младший бит каждого регистра
        r1 ^= keyBit;
        r2 ^= keyBit;
        r3 ^= keyBit;
    }

    // Загрузка 22-битного номера фрейма (побитово, без тактирования по большинству)
    for (int i = 0; i < 22; i++)
    {
        // Тактируем все три регистра принудительно
        clockR1(r1);
        clockR2(r2);
        clockR3(r3);

        // Получаем текущий бит номера фрейма
        uint8_t frameBit = (frameNumber >> i) & 1;

        // XOR бита фрейма в младший бит каждого регистра
        r1 ^= frameBit;
        r2 ^= frameBit;
        r3 ^= frameBit;
    }

    // 100 тактов прогрева по принципу большинства (без генерации выхода)
    for (int i = 0; i < 100; i++)
    {
        clockMajority(r1, r2, r3);
    }
}

// Генерация одного бита гаммы (выходной бит)
// Выходной бит = XOR старших битов трёх регистров: r1[18] ^ r2[21] ^ r3[22]
uint8_t generateBit(uint32_t& r1, uint32_t& r2, uint32_t& r3)
{
    // Тактирование по принципу большинства
    clockMajority(r1, r2, r3);

    // Получение выходных битов из старших позиций каждого регистра
    uint8_t out1 = getBit(r1, 18);
    uint8_t out2 = getBit(r2, 21);
    uint8_t out3 = getBit(r3, 22);

    return out1 ^ out2 ^ out3; // Выходной бит гаммы
}

// Генерация потока байт гаммы заданной длины
void generateKeystream(uint32_t& r1, uint32_t& r2, uint32_t& r3, 
                       vector<uint8_t>& keystream, size_t length)
{
    keystream.resize(length);
    for (size_t i = 0; i < length; i++)
    {
        uint8_t byte = 0;
        // Генерируем 8 бит для одного байта
        for (int bit = 0; bit < 8; bit++)
        {
            byte |= (generateBit(r1, r2, r3) << bit);
        }
        keystream[i] = byte;
    }
}