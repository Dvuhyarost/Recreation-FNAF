#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <vector>
#include <chrono>
#include <queue>
#include <climits>
#include "Camera.hpp"

struct PathNode {
    int x, y;
    int cost;
    bool operator>(const PathNode& other) const {
        return cost > other.cost;
    }
};

struct CameraRoute {
    int currentCamera;
    std::vector<std::pair<int, int>> nextOptions;
};

enum class EnemyType { CHICA, FREDDY, BONNIE };
enum class EnemyState { 
    SPAWNING, 
    WAITING,
    MOVING_TO_CAMERA,
    MOVING_THROUGH_DOOR,
    MOVING_TO_PLAYER
};

class Enemy {
public:
    EnemyType type;
    int x, y;
    EnemyState state = EnemyState::SPAWNING;
    
    std::chrono::steady_clock::time_point spawnTime;
    std::chrono::steady_clock::time_point waitTime;
    std::chrono::steady_clock::time_point lastTargetUpdateTime;
    
    int spawnDelaySeconds;
    int moveIntervalMs;
    int waitDuration;
    
    int targetX = 0, targetY = 0;
    int targetDoorId = -1;

    Enemy(EnemyType t, int startX, int startY);

    void update(bool officeDoorOpen, int playerX, int playerY, bool playerInOffice,
            int doorX, int doorY, bool (*isWallFunc)(int, int));
    
    bool collidesWith(int px, int py) const {
        return !(x + 2 < px || x > px + 2 || y + 1 < py || y > py + 1);
    }
    const Camera* getCurrentCamera() const; 
private:
    int nextCameraAfterDoor = -1;
    
    std::vector<std::pair<int, int>> currentPath;
    size_t currentPathIndex = 0;
    
    void findNearestFreePosition(int targetX, int targetY, bool (*isWallFunc)(int, int));
    bool canStandAt(int x, int y, bool (*isWallFunc)(int, int)) const;
    void moveToNextCamera();
    std::vector<std::pair<int, int>> findPath(int startX, int startY, int targetX, int targetY, 
                                            bool (*isWallFunc)(int, int));
    int getCurrentZone() const;
    bool shouldChasePlayer(int playerX, int playerY, bool playerInOffice) const;
    void followPath(bool (*isWallFunc)(int, int));
    size_t findCurrentRouteIndex() const;
};

bool isAreaAllowedForEnemy(EnemyType type, int areaId);

#endif // ENEMY_HPP