#include "filef.h"

// Читает файл в бинарном режиме
bool readFile(const string& path, string& content) {
    if (fs::is_directory(path)) {
        cerr << "Ошибка: '" << path << "' является директорией, укажите путь к файлу!\n";
        return false;
    }
    ifstream f(path, ios::binary);
    if (!f.is_open()) {
        cerr << "Ошибка: файл '" << path << "' не найден!\n";
        return false;
    }
    content.assign(istreambuf_iterator<char>(f), istreambuf_iterator<char>());
    f.close();
    return true;
}

// Записывает файл в бинарном режиме
bool writeFile(const string& path, const string& content) {
    ofstream f(path, ios::binary);
    if (!f.is_open()) {
        cerr << "Ошибка: не удалось создать файл '" << path << "'!\n";
        return false;
    }
    f << content;
    f.close();
    cout << "Файл сохранён в '" << path << "'\n";
    return true;
}

// Сохранение матрицы ключа Хилла в файл
void saveKey(const Matrix& K, const string& filename) {
    ofstream f(filename);
    if (!f) { cerr << "Ошибка: не удалось открыть файл " << filename << "\n"; return; }
    f << K.size() << "\n";
    for (auto& row : K) {
        for (int i = 0; i < (int)row.size(); i++) {
            f << row[i];
            if (i + 1 < (int)row.size()) f << " ";
        }
        f << "\n";
    }
}

// Загрузка матрицы ключа Хилла из файла
Matrix loadKey(const string& filename) {
    ifstream f(filename);
    if (!f) { cerr << "Ошибка: не удалось открыть файл " << filename << "\n"; return {}; }
    int n;
    f >> n;
    Matrix K(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            f >> K[i][j];
    return K;
}

void saveXorKey(const string& key, const string& filename) {
    ofstream f(filename);
    if (!f) { cerr << "Ошибка: не удалось открыть файл " << filename << "\n"; return; }
    f << key;
}

string loadXorKey(const string& filename) {
    ifstream f(filename);
    if (!f) { cerr << "Ошибка: не удалось открыть файл " << filename << "\n"; return ""; }
    string key;
    getline(f, key);
    return key;
}