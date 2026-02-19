#include "Game.hpp"
#include "ConsoleUtils.hpp"
#include <iostream>
#include <thread>

int main() {
    setupConsole();

#ifdef _WIN32
    setupConsoleWindow();
#else
    std::cout << "\nℹ️  Для корректного отображения установите размер терминала:\n";
    std::cout << "    Ширина: 120 символов, Высота: 45 строк\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
#endif

    setvbuf(stdout, nullptr, _IONBF, 0);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

#ifndef _WIN32
    extern struct TermiosRestorer termRestorer;
    termRestorer.setRaw();
#endif

    clearScreen();
    std::cout << "\n\n";
    std::cout << "         FNAF Console Edition \n\n";
    std::cout << "        Управляйте персонажем с помощью WASD\n";
    std::cout << "        Закройте двери, чтобы спастись от врага!\n\n";
    std::cout << "        Нажмите ENTER, чтобы начать...\n";

    while (true) {
        if (kbhit()) {
            char c = getch_noblock();
            if (c == '\r' || c == '\n') {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Game game;
    game.run();

    return 0;
}