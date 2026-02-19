#include "ConsoleUtils.hpp"
#include <iostream>
#include <cstdio>

#ifndef _WIN32
    #include <unistd.h>
    #include <termios.h>
    #include <poll.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mOut;
    GetConsoleMode(hOut, &mOut);
    SetConsoleMode(hOut, mOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mIn;
    GetConsoleMode(hIn, &mIn);
    SetConsoleMode(hIn, mIn & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
#endif
}

void clearScreen() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    COORD homeCoords = { 0, 0 };

    if (hOut == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;

    int cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacterA(hOut, ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hOut, homeCoords);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

#ifdef _WIN32
void setupConsoleWindow() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    COORD bufferSize = {120, 100};
    SetConsoleScreenBufferSize(hOut, bufferSize);

    SMALL_RECT windowSize = {0, 0, 119, 44};
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);

    HWND hwnd = GetConsoleWindow();
    if (hwnd) {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int winWidth = 120 * 8;
        int winHeight = 45 * 16;
        int x = (screenWidth - winWidth) / 2;
        int y = (screenHeight - winHeight) / 2;
        SetWindowPos(hwnd, HWND_TOP, x, y, winWidth, winHeight, SWP_NOZORDER);
    }
}
#endif

// === Кроссплатформенный ввод ===
#ifndef _WIN32
struct TermiosRestorer {
    struct termios oldTermios;
    bool saved = false;
    ~TermiosRestorer() {
        if (saved) tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
    }
    void setRaw() {
        struct termios t;
        tcgetattr(STDIN_FILENO, &oldTermios);
        t = oldTermios;
        t.c_lflag &= ~(ICANON | ECHO);
        t.c_cc[VMIN] = 0;
        t.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        saved = true;
    }
};
static TermiosRestorer termRestorer;
#endif

bool kbhit() {
#ifdef _WIN32
    return WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 0) == WAIT_OBJECT_0;
#else
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    return poll(&pfd, 1, 0) > 0;
#endif
}

char getch_noblock() {
#ifdef _WIN32
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD count = 0;
    if (!GetNumberOfConsoleInputEvents(hIn, &count) || count == 0) {
        return 0;
    }

    INPUT_RECORD ir;
    DWORD read;
    if (ReadConsoleInputA(hIn, &ir, 1, &read) && ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
        return ir.Event.KeyEvent.uChar.AsciiChar;
    }
    return 0;
#else
    char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    return (n == 1) ? c : 0;
#endif
}