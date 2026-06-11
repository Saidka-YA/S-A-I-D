/*
 * main.cpp — точка входа.
 * Загружает .so плагины в runtime через dlopen/dlsym.
 * .so ищутся в той же папке, что и сам бинарник (через /proc/self/exe).
 */
#include "plugin_api.h"
#include "console_utils.h"
#include <iostream>
#include <string>
#include <limits>
#include <csignal>
#include <cstdlib>
#include <dlfcn.h>
#include <unistd.h>
#include <climits>
#include <libgen.h>   // dirname
#include <cstring>

using namespace std;

// ── Список плагинов ────────────────────────────────────────────────────────
struct PluginEntry {
    const char* label;
    const char* soname;
};

static const PluginEntry PLUGINS[] = {
    { "AES-128 (CFB)",  "libaes.so"          },
    { "A5/1",           "liba51.so"           },
    { "RSA",            "librsa.so"           },
    { "Хилл / Вернам",  "libhill_vernam.so"   },
    { "Бифид / ADFGX",  "libbifid_adfgx.so"  },
    { "Аффинный шифр",  "libaffine.so"        },
    { "Шифр Плейфера",  "libplayfair.so"      },
};
static const int PLUGIN_COUNT = 7;

// ── Получить папку, в которой лежит сам исполняемый файл ──────────────────
static string get_exe_dir() {
    char buf[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n <= 0) return ".";
    buf[n] = '\0';
    // dirname модифицирует буфер — работаем с копией
    char tmp[PATH_MAX];
    strncpy(tmp, buf, PATH_MAX);
    return string(dirname(tmp));
}

// ── Утилиты ввода ──────────────────────────────────────────────────────────
static int read_int(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= lo && v <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        cout << "Введите число от " << lo << " до " << hi << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ── Загрузка и вызов плагина ───────────────────────────────────────────────
static void run_plugin(const string& exe_dir, const PluginEntry& pe) {
    string sopath = exe_dir + "/" + pe.soname;

    void* handle = dlopen(sopath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        cerr << "Не удалось загрузить " << sopath << "\n"
             << dlerror() << "\n";
        return;
    }

    dlerror();
    plugin_run_fn fn = reinterpret_cast<plugin_run_fn>(dlsym(handle, "plugin_run"));
    const char* err = dlerror();
    if (err) {
        cerr << "Символ plugin_run не найден: " << err << "\n";
        dlclose(handle);
        return;
    }

    fn();
    dlclose(handle);
}

// ── Сигнал ─────────────────────────────────────────────────────────────────
static void on_sigint(int) {
    cout << "\n\nПрограмма завершена пользователем.\n";
    exit(0);
}

// ── main ───────────────────────────────────────────────────────────────────
int main() {
    signal(SIGINT, on_sigint);
    setupConsoleUTF8();

    string exe_dir = get_exe_dir();

    cout << "╔════════════════════════════════════════╗\n";
    cout << "║       КРИПТОГРАФИЧЕСКИЕ ШИФРЫ          ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";

    int choice = -1;
    while (choice != 0) {
        cout << "─── Выберите группу шифров ───\n";
        for (int i = 0; i < PLUGIN_COUNT; ++i)
            cout << "  " << (i + 1) << ". " << PLUGINS[i].label << "\n";
        cout << "  0. Выход\n";

        choice = read_int("Выбор: ", 0, PLUGIN_COUNT);
        if (choice == 0) { cout << "До свидания!\n"; break; }

        run_plugin(exe_dir, PLUGINS[choice - 1]);

        cout << "\nНажмите Enter для возврата в меню...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n";
    }

    return 0;
}
