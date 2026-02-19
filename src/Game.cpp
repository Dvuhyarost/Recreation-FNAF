#include "Game.hpp"
#include "WallParser.hpp"
#include "ConsoleUtils.hpp"
#include "Areas.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <ctime>


const char* reset   = "\033[0m";
const char* purple  = "\033[35m";
const char* white   = "\033[97m";
const char* gray    = "\033[90m";
const char* red     = "\033[31m";
const char* green   = "\033[32m";

const int Game::WIDTH = 120;
const int Game::HEIGHT = 43;
const int Game::ROOM_LEFT = 44;
const int Game::ROOM_RIGHT = 67;
const int Game::ROOM_TOP_Y = 33;
const int Game::ROOM_BOTTOM_Y = 41;
const int Game::DOOR_HEIGHT = 5;
const int Game::DOOR_TOP_OFFSET = 2;

const int Game::LEFT_TRIGGER_X1 = Game::ROOM_LEFT + 2;
const int Game::LEFT_TRIGGER_X2 = Game::LEFT_TRIGGER_X1 + 1;
const int Game::LEFT_TRIGGER_Y1 = Game::ROOM_BOTTOM_Y - 2;
const int Game::LEFT_TRIGGER_Y2 = Game::ROOM_BOTTOM_Y - 1;

const int Game::RIGHT_TRIGGER_X1 = Game::ROOM_RIGHT - 3;
const int Game::RIGHT_TRIGGER_X2 = Game::RIGHT_TRIGGER_X1 + 1;
const int Game::RIGHT_TRIGGER_Y1 = Game::ROOM_BOTTOM_Y - 2;
const int Game::RIGHT_TRIGGER_Y2 = Game::ROOM_BOTTOM_Y - 1;
const int Game::RIGHT_DOOR_X = Game::ROOM_RIGHT + 1; // Перед дверью
const int Game::RIGHT_DOOR_Y = Game::ROOM_TOP_Y + 2; // В проходе

std::vector<std::pair<int, int>> Game::walls;


const std::vector<Area> AREAS = {
    {SHOW_STAGE,     "ShowStage",     36, 75,  0, 5,   "\033[0m"},
    {DINING_AREA,    "DiningArea",    21, 91,  5, 23,  "\033[0m"},
    {PIRATE_COVE,    "PirateCove",    8, 21, 16, 21,   "\033[0m"},
    {RESTROOMS,      "Restrooms",     91, 104, 8, 22,  "\033[0m"},
    {WEST_HALL,      "WestHall",      31, 43, 23, 40,  "\033[0m"},
    {EAST_HALL,      "EastHall",      68, 80, 23, 40,  "\033[0m"},
    {KITCHEN,        "Kitchen",       83, 100, 23, 29, "\033[0m"},
    {BACKSTAGE,      "Backstage",     8, 21, 6, 12,    "\033[0m"},
    {SUPPLY_CLOSET,  "SupplyCloset",  19, 30, 30, 36,  "\033[0m"},
    {THE_OFFICE,     "TheOffice",     44, 67, 36, 39,  "\033[0m"}
};

const char* getCameraColor(int cameraId) {
    switch (cameraId) {
        case 10: case 11: case 12: return "\033[43m"; // ShowStage - жёлтый
        case 13: return "\033[44m"; // 1B - синий
        case 14: return "\033[45m"; // 5 - фиолетовый
        case 15: return "\033[100m"; // 1C - серый
        case 16: return "\033[42m"; // 2A - зелёный
        case 17: return "\033[102m"; // 4A - светло-зелёный
        case 18: return "\033[41m"; // 2B - красный
        case 19: return "\033[101m"; // 4B - розовый
        case 20: return "\033[104m"; // 3 - голубой
        case 21: return "\033[46m"; // 7 - бирюзовый
        case 22: return "\033[103m"; // 6 - жёлтый
        default: return "\033[0m";
    }
}

const char* getAreaColor(int x, int y) {
    for (const auto& area : AREAS) {
        if (area.contains(x, y)) {
            return area.color;
        }
    }
    return "\033[0m";
}

bool Game::Player::isValidPosition() const {
    return x >= 0 && y >= 0 && x + 2 < Game::WIDTH && y + 1 < Game::HEIGHT;
}

bool Game::Player::overlapsWith(int x1, int y1, int x2, int y2) const {
    return !(maxX() < x1 || minX() > x2 || maxY() < y1 || minY() > y2);
}

Game::Game() : 
    player{(ROOM_LEFT + ROOM_RIGHT) / 2 - 1, (ROOM_TOP_Y + ROOM_BOTTOM_Y) / 2},
    chica{EnemyType::CHICA, 66, 5},
    freddy{EnemyType::FREDDY, 56, 5},
    bonnie{EnemyType::BONNIE, 46, 5}
{
    initWalls();
}

void Game::initWalls() {
    walls = loadWalls(WIDTH, HEIGHT);
}

bool Game::isWallAt(int x, int y) const {
    for (const auto& [wx, wy] : walls) {
        if (wx == x && wy == y) return true;
    }
    return false;
}

bool Game::isInsideRoom(int x, int y) const {
    return x > ROOM_LEFT && x < ROOM_RIGHT &&
           y > ROOM_TOP_Y && y < ROOM_BOTTOM_Y;
}

void Game::updateTriggers() {
    leftTriggerActive = player.overlapsWith(LEFT_TRIGGER_X1, LEFT_TRIGGER_Y1, LEFT_TRIGGER_X2, LEFT_TRIGGER_Y2);
    rightTriggerActive = player.overlapsWith(RIGHT_TRIGGER_X1, RIGHT_TRIGGER_Y1, RIGHT_TRIGGER_X2, RIGHT_TRIGGER_Y2);
    leftDoorOpen = !leftTriggerActive;
    rightDoorOpen = !rightTriggerActive;
}

bool Game::canMoveTo(const Player& p) const {
    for (int dy = 0; dy <= 1; ++dy) {
        for (int dx = 0; dx <= 2; ++dx) {
            int x = p.x + dx;
            int y = p.y + dy;

            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
                return false;

            if (isInsideRoom(x, y))
                continue;

            if (x == ROOM_LEFT && y >= ROOM_TOP_Y + DOOR_TOP_OFFSET && 
                y < ROOM_TOP_Y + DOOR_TOP_OFFSET + DOOR_HEIGHT) {
                if (!leftDoorOpen) return false;
                continue;
            }

            if (x == ROOM_RIGHT && y >= ROOM_TOP_Y + DOOR_TOP_OFFSET && 
                y < ROOM_TOP_Y + DOOR_TOP_OFFSET + DOOR_HEIGHT) {
                if (!rightDoorOpen) return false;
                continue;
            }

            if (isWallAt(x, y))
                return false;
        }
    }
    return true;
}

void Game::handleInput(char key) {
    Player newPlayer = player;
    bool moved = false;

    if (key == 'a' || key == 'A') { newPlayer.x--; moved = true; }
    else if (key == 'd' || key == 'D') { newPlayer.x++; moved = true; }
    else if (key == 'w' || key == 'W') { newPlayer.y--; moved = true; }
    else if (key == 's' || key == 'S') { newPlayer.y++; moved = true; }

    if (moved && newPlayer.isValidPosition() && canMoveTo(newPlayer)) {
        player = newPlayer;
    }
}

bool Game::isPlayerInOffice() const {
    return player.x >= 36 && player.x <= 75 && 
           player.y >= 33 && player.y <= 41;
}

const Camera* getCameraById(int x, int y) {
    for (const auto& cam : CAMERAS) {
        if (cam.x == x && cam.y == y) {
            return &cam;
        }
    }
    return nullptr;
}

void Game::drawScene() {
    std::string screen;
    screen.reserve(WIDTH * HEIGHT * 10);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            bool drawn = false;

            // Игрок
            if (y == player.y && x >= player.x && x <= player.x + 2) {
                if (x == player.x) screen += purple + std::string("▀") + reset;
                else if (x == player.x + 1) screen += purple + std::string("█") + reset;
                else if (x == player.x + 2) screen += purple + std::string("▀") + reset;
                drawn = true;
            }
            else if (y == player.y + 1 && x >= player.x && x <= player.x + 2) {
                screen += purple + std::string("▀") + reset;
                drawn = true;
            }

            if (drawn) continue;

            if (y == chica.y && x >= chica.x && x <= chica.x + 2) {
                if (x == chica.x || x == chica.x + 2) {
                    screen += "\033[33m■\033[0m";
                } else if (x == chica.x + 1) {
                    screen += "\033[33m█\033[0m";
                }
                drawn = true;
            }
            else if (y == chica.y + 1 && x >= chica.x && x <= chica.x + 2) {
                screen += "\033[33m▀\033[0m";
                drawn = true;
            }

            if (drawn) continue;

            if (y == freddy.y && x >= freddy.x && x <= freddy.x + 2) {
                if (x == freddy.x || x == freddy.x + 2) {
                    screen += "\033[90m■\033[0m";
                } else if (x == freddy.x + 1) {
                    screen += "\033[90m█\033[0m";
                }
                drawn = true;
            }
            else if (y == freddy.y + 1 && x >= freddy.x && x <= freddy.x + 2) {
                screen += "\033[90m▀\033[0m";
                drawn = true;
            }

            if (drawn) continue;

            if (y == bonnie.y && x >= bonnie.x && x <= bonnie.x + 2) {
                if (x == bonnie.x || x == bonnie.x + 2) {
                    screen += "\033[35m■\033[0m";
                } else if (x == bonnie.x + 1) {
                    screen += "\033[35m█\033[0m";
                }
                drawn = true;
            }
            else if (y == bonnie.y + 1 && x >= bonnie.x && x <= bonnie.x + 2) {
                screen += "\033[35m▀\033[0m";
                drawn = true;
            }

            if (drawn) continue;

            // Отрисовка камер как цветных точек
            for (const auto& cam : CAMERAS) {
                if (x == cam.x && y == cam.y) {
                    const char* camColor = "\033[47m"; // белый по умолчанию
                    switch (cam.id) {
                        case 10: case 11: case 12: camColor = "\033[43m"; break; // ShowStage - жёлтый
                        case 13: camColor = "\033[44m"; break; // 1B - синий
                        case 14: camColor = "\033[45m"; break; // 5 - фиолетовый
                        case 16: case 18: camColor = "\033[42m"; break; // West Hall - зелёный
                        case 17: case 19: camColor = "\033[102m"; break; // East Hall - светло-зелёный
                        case 20: camColor = "\033[104m"; break; // Supply Closet - голубой
                        case 21: camColor = "\033[41m"; break; // Restrooms - красный
                        case 22: camColor = "\033[101m"; break; // Kitchen - розовый
                    }
                    screen += camColor + std::string("•") + reset;
                    drawn = true;
                    break;
                }
            }

            if (drawn) continue;


            if (isWallAt(x, y)) {
                screen += white + std::string("█") + reset;
                drawn = true;
            }

            if (drawn) continue;

            extern const std::vector<Door> DOORS;
            for (const auto& door : DOORS) {
                if (door.contains(x, y)) {
                    const char* doorColor = "\033[36m"; // бирюзовый по умолчанию
                    if (door.id == 100) doorColor = "\033[32m"; // Show Stage - зелёный+
                    else if (door.id == 101) doorColor = "\033[34m"; // West Hall - синий + 
                    else if (door.id == 102) doorColor = "\033[35m"; // East Hall - фиолетовый+
                    else if (door.id == 103) doorColor = "\033[31m"; // Restrooms - красный+
                    else if (door.id == 104) doorColor = "\033[33m"; // Kitchen - жёлтый+
                    else if (door.id == 105) doorColor = "\033[36m"; // Supply - голубой+
                    else if (door.id >= 106) doorColor = "\033[31m"; // Office - белый
                    
                    screen += doorColor + std::string("█") + reset;
                    drawn = true;
                    break;
                }
            }
            if (drawn) continue;

            if (y >= ROOM_TOP_Y && y <= ROOM_BOTTOM_Y) {
                if (x == ROOM_LEFT && y >= ROOM_TOP_Y + DOOR_TOP_OFFSET && 
                    y < ROOM_TOP_Y + DOOR_TOP_OFFSET + DOOR_HEIGHT) {
                    if (leftDoorOpen) {
                        screen += ' ';
                    } else {
                        screen += gray + std::string("█") + reset;
                    }
                    drawn = true;
                }

                if (x == ROOM_RIGHT && y >= ROOM_TOP_Y + DOOR_TOP_OFFSET && 
                    y < ROOM_TOP_Y + DOOR_TOP_OFFSET + DOOR_HEIGHT) {
                    if (rightDoorOpen) {
                        screen += ' ';
                    } else {
                        screen += gray + std::string("█") + reset;
                    }
                    drawn = true;
                }

                if (!drawn && y >= LEFT_TRIGGER_Y1 && y <= LEFT_TRIGGER_Y2 &&
                    x >= LEFT_TRIGGER_X1 && x <= LEFT_TRIGGER_X2) {
                    screen += (leftTriggerActive ? green : red) + std::string("█") + reset;
                    drawn = true;
                }
                if (!drawn && y >= RIGHT_TRIGGER_Y1 && y <= RIGHT_TRIGGER_Y2 &&
                    x >= RIGHT_TRIGGER_X1 && x <= RIGHT_TRIGGER_X2) {
                    screen += (rightTriggerActive ? green : red) + std::string("█") + reset;
                    drawn = true;
                }
            }

            if (!drawn) {
                screen += ' ';
            }
        }
        screen += '\n';
    }

    screen += "\n=== DEBUG INFO ===\n";
    screen += "Player: (" + std::to_string(player.x) + ", " + std::to_string(player.y) + ") ";
    screen += "InOffice: " + std::string(isPlayerInOffice() ? "YES" : "NO") + "\n";

    auto getDoorName = [](int doorId) -> std::string {
        switch (doorId) {
            case DOOR_SHOW_DINING: return "SHOW_DINING";
            case DOOR_DINING_WEST: return "DINING_WEST";
            case DOOR_DINING_EAST: return "DINING_EAST";
            case DOOR_RESTROOMS_DINING: return "RESTROOMS";
            case DOOR_KITCHEN_DINING: return "KITCHEN";
            case DOOR_SUPPLY_WEST: return "SUPPLY";
            case DOOR_BACKSTAGE_DINING: return "BACKSTAGE";
            case DOOR_OFFICE_LEFT: return "OFFICE_LEFT";
            case DOOR_OFFICE_RIGHT: return "OFFICE_RIGHT";
            default: return "UNKNOWN";
        }
    };

    screen += "Chica: (" + std::to_string(chica.x) + ", " + std::to_string(chica.y) + ") ";
    const Camera* chicaCam = chica.getCurrentCamera();
    if (chicaCam) {
        screen += "At: " + chicaCam->name + " ";
    }
    switch (chica.state) {
        case EnemyState::WAITING: 
            screen += "[WAITING]"; 
            break;
        case EnemyState::MOVING_TO_CAMERA: {
            const Camera* targetCam = getCameraById(chica.targetX, chica.targetY);
            if (targetCam) {
                screen += "[→ CAMERA " + targetCam->name + "]";
            } else {
                screen += "[→ (" + std::to_string(chica.targetX) + "," + std::to_string(chica.targetY) + ")]";
            }
            break;
        }
        case EnemyState::MOVING_THROUGH_DOOR: {
            extern const std::vector<Door> DOORS;
            int targetDoorId = -1;
            for (const auto& door : DOORS) {
                if (door.getCenterX() == chica.targetX && door.getCenterY() == chica.targetY) {
                    targetDoorId = door.id;
                    break;
                }
            }
            if (targetDoorId != -1) {
                screen += "[→ DOOR " + getDoorName(targetDoorId) + "]";
            } else {
                screen += "[→ DOOR (" + std::to_string(chica.targetX) + "," + std::to_string(chica.targetY) + ")]";
            }
            break;
        }
        case EnemyState::MOVING_TO_PLAYER: 
            screen += "[→ PLAYER!]"; 
            break;
        default: 
            screen += "[OTHER]";
    }
    screen += "\n";

    screen += "Freddy: (" + std::to_string(freddy.x) + ", " + std::to_string(freddy.y) + ") ";
    const Camera* freddyCam = freddy.getCurrentCamera();
    if (freddyCam) {
        screen += "At: " + freddyCam->name + " ";
    }
    switch (freddy.state) {
        case EnemyState::WAITING: 
            screen += "[WAITING]"; 
            break;
        case EnemyState::MOVING_TO_CAMERA: {
            const Camera* targetCam = getCameraById(freddy.targetX, freddy.targetY);
            if (targetCam) {
                screen += "[→ CAMERA " + targetCam->name + "]";
            } else {
                screen += "[→ (" + std::to_string(freddy.targetX) + "," + std::to_string(freddy.targetY) + ")]";
            }
            break;
        }
        case EnemyState::MOVING_THROUGH_DOOR: {
            extern const std::vector<Door> DOORS;
            int targetDoorId = -1;
            for (const auto& door : DOORS) {
                if (door.getCenterX() == freddy.targetX && door.getCenterY() == freddy.targetY) {
                    targetDoorId = door.id;
                    break;
                }
            }
            if (targetDoorId != -1) {
                screen += "[→ DOOR " + getDoorName(targetDoorId) + "]";
            } else {
                screen += "[→ DOOR (" + std::to_string(freddy.targetX) + "," + std::to_string(freddy.targetY) + ")]";
            }
            break;
        }
        case EnemyState::MOVING_TO_PLAYER: 
            screen += "[→ PLAYER!]"; 
            break;
        default: 
            screen += "[OTHER]";
    }
    screen += "\n";
    int chicaZone = -1, freddyZone = -1, bonnieZone = -1;
    for (const auto& area : AREAS) {
        if (area.contains(chica.x, chica.y)) chicaZone = area.id;
        if (area.contains(freddy.x, freddy.y)) freddyZone = area.id;
        if (area.contains(bonnie.x, bonnie.y)) bonnieZone = area.id;
    }

    screen += "Bonnie: (" + std::to_string(bonnie.x) + ", " + std::to_string(bonnie.y) + ") ";
    const Camera* bonnieCam = bonnie.getCurrentCamera();
    if (bonnieCam) {
        screen += "At: " + bonnieCam->name + " ";
    }
    switch (bonnie.state) {
        case EnemyState::WAITING: 
            screen += "[WAITING]"; 
            break;
        case EnemyState::MOVING_TO_CAMERA: {
            const Camera* targetCam = getCameraById(bonnie.targetX, bonnie.targetY);
            if (targetCam) {
                screen += "[→ CAMERA " + targetCam->name + "]";
            } else {
                screen += "[→ (" + std::to_string(bonnie.targetX) + "," + std::to_string(bonnie.targetY) + ")]";
            }
            break;
        }
        case EnemyState::MOVING_THROUGH_DOOR: {
            extern const std::vector<Door> DOORS;
            int targetDoorId = -1;
            for (const auto& door : DOORS) {
                if (door.getCenterX() == bonnie.targetX && door.getCenterY() == bonnie.targetY) {
                    targetDoorId = door.id;
                    break;
                }
            }
            if (targetDoorId != -1) {
                screen += "[→ DOOR " + getDoorName(targetDoorId) + "]";
            } else {
                screen += "[→ DOOR (" + std::to_string(bonnie.targetX) + "," + std::to_string(bonnie.targetY) + ")]";
            }
            break;
        }
        case EnemyState::MOVING_TO_PLAYER: 
            screen += "[→ PLAYER!]"; 
            break;
        default: 
            screen += "[OTHER]";
    }
    screen += "\n";

    screen += "\nУправление: WASD — движение, Q — выход\n";
    screen += "Подойдите к КРАСНОЙ кнопке, чтобы ЗАКРЫТЬ дверь\n";
    clearScreen();
    std::cout << screen << std::flush;
}

bool Game::isWallAtStatic(int x, int y) {
    for (const auto& [wx, wy] : walls) {
        if (wx == x && wy == y) return true;
    }
    return false;
}

void Game::run() {
    while (true) {
        if (gameOver) {
            clearScreen();
            std::cout << "\n Game Over! Враг поймал вас!\n" << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            break;
        }

        char key = getch_noblock();
        if (key != 0 && key != '\0') {
            if (key == 'q' || key == 'Q') {
                clearScreen();
                std::cout << "Спасибо за игру!\n" << std::flush;
                break;
            }
            handleInput(key);
            updateTriggers();
        }

        int leftOfficeX = 45, leftOfficeY = 37;
        int rightOfficeX = 66, rightOfficeY = 37;


        chica.update(rightDoorOpen, player.x, player.y, isPlayerInOffice(), 
                    66, 37, Game::isWallAtStatic);
        freddy.update(rightDoorOpen, player.x, player.y, isPlayerInOffice(), 
                    66, 37, Game::isWallAtStatic);
        bonnie.update(leftDoorOpen, player.x, player.y, isPlayerInOffice(), 
                    45, 37, Game::isWallAtStatic);
                    
        if (chica.collidesWith(player.x, player.y) || 
            freddy.collidesWith(player.x, player.y) || 
            bonnie.collidesWith(player.x, player.y)) {
            gameOver = false;
        }

        drawScene();
        std::this_thread::sleep_for(std::chrono::milliseconds(25)); // ~20 FPS
    }
}