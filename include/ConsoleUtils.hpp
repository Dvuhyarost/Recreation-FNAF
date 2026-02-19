#ifndef CONSOLE_UTILS_HPP
#define CONSOLE_UTILS_HPP

#include <string>

void setupConsole();
void clearScreen();

#ifdef _WIN32
    void setupConsoleWindow();
#endif

// Кроссплатформенный ввод
bool kbhit();
char getch_noblock();

#endif // CONSOLE_UTILS_HPP