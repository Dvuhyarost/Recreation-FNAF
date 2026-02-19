#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <utility>
#include "Enemy.hpp"


class Game {
private:
    static const int WIDTH;
    static const int HEIGHT;
    
    // Параметры комнаты
    static const int ROOM_LEFT;
    static const int ROOM_RIGHT;
    static const int ROOM_TOP_Y;
    static const int ROOM_BOTTOM_Y;
    static const int DOOR_HEIGHT;
    static const int DOOR_TOP_OFFSET;

    // Триггеры
    static const int LEFT_TRIGGER_X1, LEFT_TRIGGER_X2, LEFT_TRIGGER_Y1, LEFT_TRIGGER_Y2;
    static const int RIGHT_TRIGGER_X1, RIGHT_TRIGGER_X2, RIGHT_TRIGGER_Y1, RIGHT_TRIGGER_Y2;

    // Двери
    static const int RIGHT_DOOR_X;
    static const int RIGHT_DOOR_Y;

    struct Player {
        int x, y;
        int minX() const { return x; }
        int maxX() const { return x + 2; }
        int minY() const { return y; }
        int maxY() const { return y + 1; }
        bool isValidPosition() const;
        bool overlapsWith(int x1, int y1, int x2, int y2) const;
    };

    Player player;
    Enemy chica;
    Enemy freddy;
    Enemy bonnie;
    static std::vector<std::pair<int, int>> walls; // ← static

    bool leftDoorOpen, rightDoorOpen;
    bool leftTriggerActive, rightTriggerActive;
    bool gameOver = false;

    void initWalls();
    bool isWallAt(int x, int y) const;
    bool isInsideRoom(int x, int y) const;
    void updateTriggers();
    bool canMoveTo(const Player& p) const;
    void handleInput(char key);
    void drawScene();
    bool isPlayerInOffice() const;

public:
    Game();
    void run();
    static bool isWallAtStatic(int x, int y);
};

#endif // GAME_HPP