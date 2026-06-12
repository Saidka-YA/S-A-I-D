#include "file_processor.h"
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

vector<uint8_t> FileProcessor::read_file(const string& path) {
    if (!fs::exists(path)) {
        throw runtime_error("Файл не найден: " + path);
    }
    ifstream file(path, ios::binary | ios::ate);
    if (!file) throw runtime_error("Не удалось открыть файл для чтения.");

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw runtime_error("Ошибка чтения файла.");
    }
    return buffer;
}

void FileProcessor::write_file(const string& path, const vector<uint8_t>& data) {
    if (!ensure_directory_exists(path)) {
        throw runtime_error("Не удалось создать директории для файла.");
    }

    ofstream file(path, ios::binary);
    if (!file) throw runtime_error("Не удалось создать файл для записи.");
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

bool FileProcessor::ensure_directory_exists(const string& file_path) {
    fs::path p(file_path);
    if (p.has_parent_path()) {
        fs::path dir = p.parent_path();
        if (!fs::exists(dir)) {
            cout << "Директория не найдена. Создать \"" << dir.string() << "\"? (y/n): ";
            char ans;
            cin >> ans;
            if (ans == 'y' || ans == 'Y') {
                return fs::create_directories(dir);
            }
            return false;
        }
    }
    return true;
}