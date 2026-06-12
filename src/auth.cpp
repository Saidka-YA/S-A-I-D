#include "auth.h"
#include <iostream>
#include <string>

using namespace std;

// ── Закреплённые учётные данные ────────────────────────────────────────────
static const string FIXED_LOGIN    = "admin";
static const string FIXED_PASSWORD = "admin123";

bool login() {
    string login_input, password_input;

    cout << "Логин: ";
    getline(cin, login_input);

    cout << "Пароль: ";
    getline(cin, password_input);

    if (login_input == FIXED_LOGIN && password_input == FIXED_PASSWORD) {
        cout << "\nВход выполнен успешно.\n\n";
        return true;
    }

    return false;
}
