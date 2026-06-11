#include "ui.h"
#include "library_loader.h"
#include "file_processor.h"
#include "key_generator.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <limits>
#include <csignal>
#include <cstdlib>

using namespace std;
namespace fs = std::filesystem;

// Обработчик сигнала Ctrl+C - завершает программу
void signal_handler(int signum) {
    cout << "\n\nПрограмма завершена пользователем.\n";
    exit(0);
}

// Установка обработчика сигналов
void setup_signal_handler() {
    signal(SIGINT, signal_handler);
}

string UserInterface::safe_input_string(const string& prompt) {
    string result;
    while (true) {
        cout << prompt;
        if (getline(cin, result) && !result.empty()) {
            return result;
        }
        cout << "Ввод не может быть пустым. Попробуйте снова.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int UserInterface::safe_input_int(const string& prompt, int min_val, int max_val) {
    int result;
    while (true) {
        cout << prompt;
        if (cin >> result) {
            if (result >= min_val && result <= max_val) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return result;
            }
            cout << "Число должно быть от " << min_val << " до " << max_val << ".\n";
        } else {
            cout << "Некорректный ввод. Введите число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string UserInterface::safe_input_path(const string& prompt) {
    string path;
    while (true) {
        cout << prompt << ": ";
        if (getline(cin, path)) {
            if (!path.empty()) {
                return path;
            }
        }
        cout << "Путь не может быть пустым. Попробуйте снова.\n";
        cin.clear();
    }
}

bool UserInterface::safe_input_yes_no(const string& prompt, bool default_yes) {
    string answer;
    while (true) {
        cout << prompt << " (Y/n): ";
        if (getline(cin, answer)) {
            if (answer.empty()) return default_yes;
            
            char c = tolower(answer[0]);
            if (c == 'y' || c == 'Y') return true;
            if (c == 'n' || c == 'N') return false;
        }
        cout << "Ответьте Y или N.\n";
    }
}

string UserInterface::get_auto_text_filename(bool is_encrypt) const {
    return is_encrypt ? "text_encrypted.txt" : "text_decrypted.txt";
}

string UserInterface::get_auto_output_path(const string& input_path, bool is_encrypt) const {
    fs::path p(input_path);
    string suffix = is_encrypt ? "_enc" : "_dec";
    
    fs::path parent = p.parent_path();
    if (parent.empty()) {
        parent = fs::current_path();
    }
    
    string new_name = p.stem().string() + suffix + p.extension().string();
    return (parent / new_name).string();
}

void UserInterface::run() {
    setup_signal_handler();
    int choice = -1;
    
    cout << "=== Encryption Algorithm RGR ===\n\n";
    
    while (choice != 0) {
        show_menu();
        choice = safe_input_int("Выбор: ", 0, 5);
        
        if (choice == 0) {
            cout << "Завершение работы.\n";
            break;
        }
        
        try {
            switch (choice) {
                case 1: 
                    cout << "\n--- Шифрование текста ---\n";
                    process_text(true); 
                    break;
                case 2: 
                    cout << "\n--- Расшифрование текста ---\n";
                    process_text(false); 
                    break;
                case 3: 
                    cout << "\n--- Шифрование файла ---\n";
                    process_file(true); 
                    break;
                case 4: 
                    cout << "\n--- Расшифрование файла ---\n";
                    process_file(false); 
                    break;
                case 5: 
                    cout << "\n--- Генерация ключа ---\n";
                    generate_and_save_key(); 
                    break;
            }
        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << "\n";
        }
        
        cout << "\nНажмите Enter для возврата в меню...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        cout << "\n";
    }
}

void UserInterface::show_menu() {
    cout << "Меню:\n";
    cout << "  1. Шифрование текста\n";
    cout << "  2. Расшифрование текста\n";
    cout << "  3. Шифрование файла\n";
    cout << "  4. Расшифрование файла\n";
    cout << "  5. Генерация ключа\n";
    cout << "  0. Выход\n";
}

void UserInterface::process_text(bool is_encrypt) {
    // Выбор алгоритма
    string algo = safe_input_string("Алгоритм (affine/playfair): ");
    
    try {
        LibraryLoader loader(algo);
        const AlgorithmInfo* info = loader.get_info();
        
        cout << "Размер ключа: " << info->key_size << " байт\n";
        
        // Ввод ключа (hex или путь к файлу)
        vector<uint8_t> key(info->key_size);
        string key_input;
        
        while (true) {
            key_input = safe_input_string("Ключ (hex) или путь к файлу ключа: ");
            
            // Проверяем, является ли ввод путем к файлу
            bool is_path = (key_input.find('/') != string::npos || 
                           key_input.find('\\') != string::npos ||
                           key_input.find('.') != string::npos);
            
            if (is_path && fs::exists(key_input)) {
                // Читаем ключ из файла
                try {
                    vector<uint8_t> file_key = FileProcessor::read_file(key_input);
                    if (file_key.size() < info->key_size) {
                        cout << "Ошибка: размер ключа в файле (" << file_key.size() 
                             << " байт) меньше требуемого (" << info->key_size << " байт).\n";
                        continue;
                    }
                    key = vector<uint8_t>(file_key.begin(), file_key.begin() + info->key_size);
                    break;
                } catch (const exception& e) {
                    cout << "Ошибка чтения файла ключа: " << e.what() << "\n";
                    continue;
                }
            } else {
                // Пытаемся интерпретировать как hex-строку
                string clean_key;
                for (char c : key_input) {
                    if (c != ' ') clean_key += tolower(c);
                }
                
                if (clean_key.length() != info->key_size * 2) {
                    cout << "Ошибка: требуется " << info->key_size * 2 
                         << " hex-символов (сейчас " << clean_key.length() << ").\n";
                    continue;
                }
                
                bool valid = true;
                for (char c : clean_key) {
                    if (!isxdigit(c)) {
                        valid = false;
                        break;
                    }
                }
                
                if (!valid) {
                    cout << "Ошибка: ключ должен содержать только hex-цифры (0-9, a-f).\n";
                    continue;
                }
                
                // Преобразуем hex в байты
                for (size_t i = 0; i < info->key_size; ++i) {
                    key[i] = static_cast<uint8_t>(stoul(clean_key.substr(i * 2, 2), nullptr, 16));
                }
                break;
            }
        }
        
        ConstBuffer k_buf{key.data(), key.size()};
        if (loader.validate_key(k_buf) != 0) {
            secure_clear_memory(key.data(), key.size());
            throw runtime_error("Ключ не подходит для данного алгоритма.");
        }
        
        // Ввод текста
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string text = safe_input_string(is_encrypt ? "Текст для шифрования: " 
                                                   : "Шифротекст: ");
        
        vector<uint8_t> input_data(text.begin(), text.end());
        size_t out_size = loader.get_output_size(input_data.size(), is_encrypt);
        vector<uint8_t> output_data(out_size);
        
        ConstBuffer in_buf{input_data.data(), input_data.size()};
        MutBuffer out_buf{output_data.data(), out_size};
        
        int status = loader.execute_crypto(is_encrypt, k_buf, in_buf, &out_buf);
        if (status != 0) {
            secure_clear_memory(key.data(), key.size());
            throw runtime_error("Ошибка операции. Код: " + to_string(status));
        }
        
        output_data.resize(out_buf.size);
        string result(output_data.begin(), output_data.end());
        
        cout << "\nРезультат:\n" << result << "\n";
        
        // Предложение сохранить в файл
        cout << "\nСохранить результат в файл? ";
        if (safe_input_yes_no("", true)) {
            string default_path = get_auto_text_filename(is_encrypt);
            string save_path = safe_input_path("Путь к файлу");
            
            try {
                // Проверка и создание директории при необходимости
                fs::path p(save_path);
                if (p.has_parent_path() && !p.parent_path().empty()) {
                    fs::path dir = p.parent_path();
                    if (!fs::exists(dir)) {
                        cout << "Директория \"" << dir.string() 
                             << "\" не существует. Создать? ";
                        if (safe_input_yes_no("", true)) {
                            fs::create_directories(dir);
                            cout << "Директория создана.\n";
                        } else {
                            throw runtime_error("Директория не создана. Сохранение отменено.");
                        }
                    }
                }
                
                ofstream file(save_path, ios::binary);
                if (!file) {
                    throw runtime_error("Не удалось открыть файл для записи.");
                }
                file.write(reinterpret_cast<const char*>(output_data.data()), 
                           output_data.size());
                file.close();
                
                cout << "Результат сохранён в: " << save_path << "\n";
                
            } catch (const exception& e) {
                cerr << "Ошибка сохранения: " << e.what() << "\n";
            }
        }
        
        secure_clear_memory(key.data(), key.size());
        
    } catch (const exception& e) {
        throw;
    }
}

void UserInterface::process_file(bool is_encrypt) {
    // Алгоритм
    string algo = safe_input_string("Алгоритм (affine/playfair): ");
    
    try {
        LibraryLoader loader(algo);
        const AlgorithmInfo* info = loader.get_info();
        
        // Входной файл
        string input_path = safe_input_path("Входной файл");
        
        if (!fs::exists(input_path)) {
            throw runtime_error("Входной файл не существует: " + input_path);
        }
        
        // Ключ (hex или путь к файлу)
        vector<uint8_t> key(info->key_size);
        string key_input;
        
        cout << "Размер ключа: " << info->key_size << " байт\n";
        
        while (true) {
            key_input = safe_input_string("Ключ (hex) или путь к файлу ключа: ");
            
            // Проверяем, является ли ввод путем к файлу
            bool is_path = (key_input.find('/') != string::npos || 
                           key_input.find('\\') != string::npos ||
                           key_input.find('.') != string::npos);
            
            if (is_path && fs::exists(key_input)) {
                // Читаем ключ из файла
                try {
                    vector<uint8_t> file_key = FileProcessor::read_file(key_input);
                    if (file_key.size() < info->key_size) {
                        cout << "Ошибка: размер ключа в файле (" << file_key.size() 
                             << " байт) меньше требуемого (" << info->key_size << " байт).\n";
                        continue;
                    }
                    key = vector<uint8_t>(file_key.begin(), file_key.begin() + info->key_size);
                    break;
                } catch (const exception& e) {
                    cout << "Ошибка чтения файла ключа: " << e.what() << "\n";
                    continue;
                }
            } else {
                // Пытаемся интерпретировать как hex-строку
                string clean_key;
                for (char c : key_input) {
                    if (c != ' ') clean_key += tolower(c);
                }
                
                if (clean_key.length() != info->key_size * 2) {
                    cout << "Ошибка: требуется " << info->key_size * 2 
                         << " hex-символов (сейчас " << clean_key.length() << ").\n";
                    continue;
                }
                
                bool valid = true;
                for (char c : clean_key) {
                    if (!isxdigit(c)) {
                        valid = false;
                        break;
                    }
                }
                
                if (!valid) {
                    cout << "Ошибка: ключ должен содержать только hex-цифры (0-9, a-f).\n";
                    continue;
                }
                
                // Преобразуем hex в байты
                for (size_t i = 0; i < info->key_size; ++i) {
                    key[i] = static_cast<uint8_t>(stoul(clean_key.substr(i * 2, 2), nullptr, 16));
                }
                break;
            }
        }
        
        ConstBuffer k_buf{key.data(), key.size()};
        if (loader.validate_key(k_buf) != 0) {
            secure_clear_memory(key.data(), key.size());
            throw runtime_error("Ключ не подходит для данного алгоритма.");
        }
        
        // Выходной файл с автоподстановкой
        string default_output = get_auto_output_path(input_path, is_encrypt);
        string output_path = safe_input_path("Выходной файл");
        if (output_path.empty()) {
            output_path = default_output;
        }
        
        // Чтение входного файла
        vector<uint8_t> input_data = FileProcessor::read_file(input_path);
        
        size_t out_size = loader.get_output_size(input_data.size(), is_encrypt);
        vector<uint8_t> output_data(out_size);
        
        ConstBuffer in_buf{input_data.data(), input_data.size()};
        MutBuffer out_buf{output_data.data(), out_size};
        
        int status = loader.execute_crypto(is_encrypt, k_buf, in_buf, &out_buf);
        if (status != 0) {
            secure_clear_memory(key.data(), key.size());
            secure_clear_memory(input_data.data(), input_data.size());
            throw runtime_error("Ошибка операции. Код: " + to_string(status));
        }
        
        output_data.resize(out_buf.size);
        
        // Запись результата с проверкой директории
        try {
            fs::path p(output_path);
            if (p.has_parent_path() && !p.parent_path().empty()) {
                fs::path dir = p.parent_path();
                if (!fs::exists(dir)) {
                    cout << "Директория \"" << dir.string() 
                         << "\" не существует. Создать? ";
                    if (safe_input_yes_no("", true)) {
                        fs::create_directories(dir);
                        cout << "Директория создана.\n";
                    } else {
                        throw runtime_error("Создание директории отменено.");
                    }
                }
            }
            
            FileProcessor::write_file(output_path, output_data);
            cout << "Готово. Результат: " << output_path << "\n";
            
        } catch (const exception& e) {
            secure_clear_memory(key.data(), key.size());
            secure_clear_memory(input_data.data(), input_data.size());
            throw;
        }
        
        secure_clear_memory(key.data(), key.size());
        secure_clear_memory(input_data.data(), input_data.size());
        
    } catch (const exception& e) {
        throw;
    }
}

void UserInterface::generate_and_save_key() {
    string algo = safe_input_string("Алгоритм (affine/playfair): ");
    
    try {
        LibraryLoader loader(algo);
        const AlgorithmInfo* info = loader.get_info();
        
        string default_name = "key_" + string(info->algorithm_name) + ".bin";
        string save_path = safe_input_path("Сохранить ключ в");
        if (save_path.empty()) {
            save_path = default_name;
        }
        
        vector<uint8_t> key;
        int attempts = 0;
        int status = -1;
        
        do {
            key = KeyGenerator::generate_secure_key(info->key_size);
            ConstBuffer k_buf{key.data(), key.size()};
            status = loader.validate_key(k_buf);
            attempts++;
        } while (status != 0 && attempts < 1000);
        
        if (status != 0) {
            secure_clear_memory(key.data(), key.size());
            throw runtime_error("Не удалось сгенерировать корректный ключ.");
        }
        
        // Создание директории при необходимости
        try {
            fs::path p(save_path);
            if (p.has_parent_path() && !p.parent_path().empty()) {
                fs::path dir = p.parent_path();
                if (!fs::exists(dir)) {
                    cout << "Директория \"" << dir.string() 
                         << "\" не существует. Создать? ";
                    if (safe_input_yes_no("", true)) {
                        fs::create_directories(dir);
                        cout << "Директория создана.\n";
                    } else {
                        throw runtime_error("Создание директории отменено.");
                    }
                }
            }
            
            FileProcessor::write_file(save_path, key);
            
            cout << "Ключ сгенерирован: " << save_path << "\n";
            cout << "Размер: " << key.size() << " байт\n";
            
        } catch (const exception& e) {
            secure_clear_memory(key.data(), key.size());
            throw;
        }
        
        secure_clear_memory(key.data(), key.size());
        
    } catch (const exception& e) {
        throw;
    }
}