#include "menu.h"
#include <iostream>
#include <limits>

using namespace std;

string input_string(const string& prompt) {
    string result;
    while (true) {
        cout << prompt;
        if (getline(cin, result) && !result.empty())
            return result;
        cout << "Ввод не может быть пустым. Попробуйте снова.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int input_int(const string& prompt, int min_val, int max_val) {
    int result;
    while (true) {
        cout << prompt;
        if (cin >> result) {
            if (result >= min_val && result <= max_val) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return result;
            }
            cout << "Введите число от " << min_val << " до " << max_val << ".\n";
        } else {
            cout << "Некорректный ввод. Введите число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string input_path(const string& prompt) {
    string path;
    while (true) {
        cout << prompt << ": ";
        if (getline(cin, path) && !path.empty())
            return path;
        cout << "Путь не может быть пустым.\n";
        cin.clear();
    }
}

bool input_yes_no(const string& prompt, bool default_yes) {
    string answer;
    while (true) {
        cout << prompt << " (Y/n): ";
        if (getline(cin, answer)) {
            if (answer.empty()) return default_yes;
            char c = tolower(answer[0]);
            if (c == 'y') return true;
            if (c == 'n') return false;
        }
        cout << "Ответьте Y или N.\n";
    }
}
