#include "ui.h"
#include <iostream>
#include <exception>
#include <clocale>

using namespace std;

int main() {
    try {
        setlocale(LC_ALL, "ru_RU.UTF-8"); 
        UserInterface ui;
        ui.run();
        return 0;
    } catch (const exception& e) {
        cerr << "\nКритическая ошибка приложения: " << e.what() << "\n";
        return 1;
    } catch (...) {
        cerr << "\nПроизошла неизвестная критическая ошибка.\n";
        return 1;
    }
}