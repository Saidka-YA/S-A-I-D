#include "console_utils.h"
#include <clocale>

#ifdef _WIN32
    #include <windows.h>
#endif

void setupConsoleUTF8() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#else
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
}