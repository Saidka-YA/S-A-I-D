#include "RSAutilities.h"

using namespace std;

namespace rsa {

// Функция генерации простого числа
void generatePrime(mpz_t prime, int bits)
{
    gmp_randstate_t state; // Состояние генератора
    gmp_randinit_default(state); // Сдандартный генератор GMP
    
    // Инициализация генератора случайных чисел
    random_device gen;
    gmp_randseed_ui(state, gen());
    
    // Генерация простого числа заданной битности
    mpz_urandomb(prime, state, bits);
    mpz_nextprime(prime, prime);
    
    gmp_randclear(state);
}

// Функция добавления байтов в конец сообщения
void Padding(vector<uint8_t>& data)
{
    // Padding до кратности 64 байтам (512 бит)
    uint8_t padding = 64 - (data.size() % 64);
    if (padding == 0) {padding = 64;} 
    for (int i = 0; i < padding; i++)
    {
        data.push_back(padding); // Добавление байтов в конец сообщения
    }    
}

// Функция удаления добавленных байтов в конец сообщения
void deletePadding(vector<uint8_t>& data)
{
    if (data.empty()) return; // Если вводные данные пусты выходим из функции, удалять нечего
    // Получаем последний байт наших данных, это как раз добавочное значение
    uint8_t padding = data.back(); 
    if (padding == 0 || padding > 64) return; // Проверка границ
    data.resize(data.size() - padding); // Удаляем добавочные байты
}

// Запись ключей в файл
bool saveKey(const string& path, const mpz_t n, const mpz_t e, const mpz_t d)
{
    // Открыть файл как бинарник
    ofstream file(path, ios::binary);
    // Проверка открытия
    if (!file) return false;
    
    // Сохраняем каждое число
    // Формат: размер (8 байт) + данные
    
    // Сохраняем n
    size_t nSize = (mpz_sizeinbase(n, 2) + 7) / 8;
    file.write(reinterpret_cast<const char*>(&nSize), sizeof(size_t));
    // Инициализируем вектор байтов числа n и записываем туда n
    vector<uint8_t> nBytes(nSize);
    mpz_export(&nBytes[0], NULL, 1, sizeof(uint8_t), 0, 0, n);
    file.write(reinterpret_cast<const char*>(&nBytes[0]), nSize);
    
    // Сохраняем e
    size_t eSize = (mpz_sizeinbase(e, 2) + 7) / 8;
    file.write(reinterpret_cast<const char*>(&eSize), sizeof(size_t));
    vector<uint8_t> eBytes(eSize);
    mpz_export(&eBytes[0], NULL, 1, sizeof(uint8_t), 0, 0, e);
    file.write(reinterpret_cast<const char*>(&eBytes[0]), eSize);
    
    // Сохраняем d
    size_t dSize = (mpz_sizeinbase(d, 2) + 7) / 8;
    file.write(reinterpret_cast<const char*>(&dSize), sizeof(size_t));
    vector<uint8_t> dBytes(dSize);
    mpz_export(&dBytes[0], NULL, 1, sizeof(uint8_t), 0, 0, d);
    file.write(reinterpret_cast<const char*>(&dBytes[0]), dSize);
    
    // Закрытие файла
    file.close();
    return true;
}

bool loadKey(const string& path, mpz_t n, mpz_t e, mpz_t d)
{
    // Открываем файл как бинарник
    ifstream file(path, ios::binary);
    // Проверка открытия
    if (!file) return false;
    
    // Загружаем n
    size_t nSize;
    file.read(reinterpret_cast<char*>(&nSize), sizeof(size_t));
    vector<uint8_t> nBytes(nSize);
    file.read(reinterpret_cast<char*>(&nBytes[0]), nSize);
    mpz_import(n, nSize, 1, sizeof(uint8_t), 0, 0, &nBytes[0]);
    
    // Загружаем e
    size_t eSize;
    file.read(reinterpret_cast<char*>(&eSize), sizeof(size_t));
    vector<uint8_t> eBytes(eSize);
    file.read(reinterpret_cast<char*>(&eBytes[0]), eSize);
    mpz_import(e, eSize, 1, sizeof(uint8_t), 0, 0, &eBytes[0]);
    
    // Загружаем d
    size_t dSize;
    file.read(reinterpret_cast<char*>(&dSize), sizeof(size_t));
    vector<uint8_t> dBytes(dSize);
    file.read(reinterpret_cast<char*>(&dBytes[0]), dSize);
    mpz_import(d, dSize, 1, sizeof(uint8_t), 0, 0, &dBytes[0]);
    
    // Закрываем файл
    file.close();
    return true;
}

// Читаем файл
bool readFile(const string& path, vector<uint8_t>& data)
{
    // Открытие файла в бинарном режиме
    ifstream file(path, ios::binary);
    // Проверка открытия файла
    if (!file) return false;
    // Перемещение в конец файла для получения размера файла
    // seekg == seek get position
    // Буквальный перевод строки: переместись на 0 байт относительно конца файла
    file.seekg(0, ios::end);
    // Получение и запись размера файла
    // Принцип: размер == последней позиции в файле
    //tellg == tell get position
    size_t size = (size_t)file.tellg();
    // Перемешение обратно в начало
    file.seekg(0, ios::beg);
    // Увеличиваем размер вектора data 
    data.resize(size);
    // Запись данных в файл
    file.read(reinterpret_cast<char*>(&data[0]), size);
    // Закрытие файла
    file.close();
    return true;
}

// Запись в файл
bool writeFile(const string& path, const vector<uint8_t>& data)
{
    // Открытие файла в бинарном режиме
    ofstream file(path, ios::binary);
    // Проверка открытия
    if (!file) return false;
    // Запись данных в файл
    file.write(reinterpret_cast<const char*>(&data[0]), data.size());
    // Закрытие файла
    file.close();
    return true;
}

// Перевод байт в hex-строку для вывода в консоль
string hexDisplay(const vector<uint8_t>& data)
{
    ostringstream oss; // Строковый поток для вывода в строку
    for (size_t i = 0; i < data.size(); i++)
    {
        // Перевод вывода в режим hex, и перевод из uint8_t в int
        oss << hex << setw(2) << setfill('0') << (int)data[i];
        if (i + 1 < data.size()) oss << " ";
    }
    return oss.str(); // Возвращаем полученную строку из потока
}

} // namespace rsa