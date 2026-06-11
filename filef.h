#ifndef FILEF_H
#define FILEF_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include "textproc.h"

using namespace std;
namespace fs = filesystem;

using Matrix = vector<vector<int>>;

// Чтение и запись файлов
bool readFile(const string& path, string& content);
bool writeFile(const string& path, const string& content);

// Файловые операции с ключом Хилла
void saveKey(const Matrix& K, const string& filename);
Matrix loadKey(const string& filename);

// файловые операции с ключом Вернама
void saveVernamKey(const vector<uint8_t>& key, const string& filename);
vector<uint8_t> loadVernamKey(const string& filename);

#endif
