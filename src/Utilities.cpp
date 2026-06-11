#include "utilities.h"
using namespace std;

namespace aes {

// Функция добавления байтов в конец сообщения
void Padding(vector<uint8_t>& data)
{
    uint8_t padding = 16 - (data.size() % 16); // Определение количества необходимых байт
    if (padding == 0) {padding = 16;} 
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
    if (padding == 0 || padding > 16) return; // Проверка границ
    data.resize(data.size() - padding); // Удаляем добавочные байты
}

// Генерация начального ключа
void generateKey(uint8_t key[16])
{
    random_device gen;
    for (int i = 0; i < 16; i++)
    {
        key[i] = gen() & 0xFF; // Генерируем число и берём только младшие 8 бит
    }
}

// Функция генерации вектора инициализации IV
void generateIV(uint8_t iv[16])
{
    random_device gen;
    for (int i = 0; i < 16; i++)
    {
        iv[i] = gen() & 0xFF; // Генерируем число и берём только младшие 8 бит
    }
}

// Запись в ключа в файл
bool saveKey(const string& path, uint8_t key[16])
{
    // Открыть файл как бинарник
    ofstream file(path, ios::binary);
    // Проверка открытия
    if (!file) return false;
    // Запись в файл
    // Используем reinterpret_cast для того чтобы компилятор думал что работает с char*
    // а не с uint8_t так как этого требует метод write()
    file.write(reinterpret_cast<const char*>(key), 16);
    // Закрытие файла
    file.close();
    return true;
}

// Загружаем ключ из файла
bool loadKey(const string& path, uint8_t key[16])
{
    // Открываем файл как бинарник
    ifstream file(path, ios::binary);
    // Проверка открытия
    if (!file) return false;
    // Читаем файл и также используем reinterpret_cast для того чтобы "обмануть" компилятор
    file.read(reinterpret_cast<char*>(key), 16);
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

} // namespace aes