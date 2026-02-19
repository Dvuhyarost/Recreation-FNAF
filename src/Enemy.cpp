#include "Enemy.hpp"
#include "Camera.hpp"
#include "Areas.hpp"
#include "Game.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <map>

bool isAreaAllowedForEnemy(EnemyType type, int areaId) {
    switch (type) {
        case EnemyType::FREDDY:
            return areaId == SHOW_STAGE || 
                   areaId == DINING_AREA || 
                   areaId == RESTROOMS || 
                   areaId == EAST_HALL || 
                   areaId == WEST_HALL || 
                   areaId == KITCHEN || 
                   areaId == BACKSTAGE || 
                   areaId == THE_OFFICE;
                   
        case EnemyType::CHICA:
            return areaId == SHOW_STAGE || 
                   areaId == DINING_AREA || 
                   areaId == RESTROOMS || 
                   areaId == EAST_HALL || 
                   areaId == KITCHEN || 
                   areaId == THE_OFFICE;
                   
        case EnemyType::BONNIE:
            return areaId == SHOW_STAGE || 
                   areaId == DINING_AREA || 
                   areaId == WEST_HALL || 
                   areaId == BACKSTAGE || 
                   areaId == SUPPLY_CLOSET || 
                   areaId == THE_OFFICE;
    }
    return false;
}

bool canEnemyMoveTo(int x, int y, EnemyType type, bool (*isWallFunc)(int, int)) {
    if (x < 0 || x + 2 >= 120 || y < 0 || y + 1 >= 43) 
        return false;
    
    extern const std::vector<Door> DOORS;
    bool inDoor = false;
    const Door* targetDoor = nullptr;
    
    for (const auto& door : DOORS) {
        for (int dy = 0; dy <= 1 && !inDoor; ++dy) {
            for (int dx = 0; dx <= 2 && !inDoor; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                if (door.contains(nx, ny)) {
                    inDoor = true;
                    targetDoor = &door;
                }
            }
        }
    }
    
    if (inDoor && targetDoor) {
        if (targetDoor->id == DOOR_OFFICE_LEFT) {
            extern bool leftDoorOpen;
            return leftDoorOpen;
        } else if (targetDoor->id == DOOR_OFFICE_RIGHT) {
            extern bool rightDoorOpen;
            return rightDoorOpen;
        }
        return true;
    }
    
    for (int dy = 0; dy <= 1; ++dy) {
        for (int dx = 0; dx <= 2; ++dx) {
            int nx = x + dx;
            int ny = y + dy;
            if (isWallFunc(nx, ny)) 
                return false;
        }
    }
    
    for (const auto& area : AREAS) {
        if (area.contains(x, y)) {
            return isAreaAllowedForEnemy(type, area.id);
        }
    }
    
    return false;
}

bool canStandAtPathfinding(int x, int y, EnemyType type, bool (*isWallFunc)(int, int)) {
    if (x < 0 || x + 2 >= 120 || y < 0 || y + 1 >= 43) 
        return false;
    
    for (int dy = 0; dy <= 1; ++dy) {
        for (int dx = 0; dx <= 2; ++dx) {
            int nx = x + dx;
            int ny = y + dy;
            if (isWallFunc(nx, ny)) 
                return false;
        }
    }
    
    for (const auto& area : AREAS) {
        if (area.contains(x, y)) {
            return isAreaAllowedForEnemy(type, area.id);
        }
    }
    
    return false;
}


const std::vector<CameraRoute> FREDDY_ROUTES = {
    {CAM_1A_FREDDY, {{DOOR_SHOW_DINING, CAM_1B}}},
    {CAM_1B, {{DOOR_RESTROOMS_DINING, CAM_7}}},
    {CAM_7, {{DOOR_RESTROOMS_DINING, CAM_1B}}},
    {CAM_1B, {{DOOR_KITCHEN_DINING, CAM_6}}},
    {CAM_6, {{DOOR_KITCHEN_DINING, CAM_1B}}},
    {CAM_1B, {{DOOR_DINING_EAST, CAM_4A}}},
    {CAM_4A, {{0, CAM_4B}}},
    {CAM_4B, {{DOOR_OFFICE_RIGHT, CAM_OFFICE}}},
    {CAM_OFFICE, {{0, CAM_1B}}}
};

const std::vector<CameraRoute> BONNIE_ROUTES = {
    {CAM_1A_BONNIE, {{DOOR_SHOW_DINING, CAM_1B}, {DOOR_BACKSTAGE_DINING, CAM_5}}},
    {CAM_5, {{DOOR_BACKSTAGE_DINING, CAM_1B}, {DOOR_DINING_WEST, CAM_2A}}},
    {CAM_1B, {{DOOR_BACKSTAGE_DINING, CAM_5}, {DOOR_DINING_WEST, CAM_2A}}},
    {CAM_2A, {{DOOR_SUPPLY_WEST, CAM_3}, {0, CAM_2B}}},
    {CAM_3, {{DOOR_SUPPLY_WEST, CAM_2A}, {DOOR_OFFICE_LEFT, CAM_OFFICE}}},
    {CAM_2B, {{DOOR_OFFICE_LEFT, CAM_OFFICE}, {0, CAM_3}}}
};

const std::vector<CameraRoute> CHICA_ROUTES = {
    {CAM_1A_CHICA, {{DOOR_SHOW_DINING, CAM_1B}, {DOOR_RESTROOMS_DINING, CAM_7}}},
    {CAM_7, {{DOOR_RESTROOMS_DINING, CAM_1B}, {DOOR_KITCHEN_DINING, CAM_6}}},
    {CAM_1B, {{DOOR_RESTROOMS_DINING, CAM_7}, {DOOR_KITCHEN_DINING, CAM_6}}},
    {CAM_6, {{DOOR_KITCHEN_DINING, CAM_1B}, {DOOR_DINING_EAST, CAM_4A}}},
    {CAM_4A, {{0, CAM_4B}}},
    {CAM_4B, {{DOOR_OFFICE_RIGHT, CAM_OFFICE}, {0, CAM_6}}}
};
std::vector<std::pair<int, int>> Enemy::findPath(int startX, int startY, int targetX, int targetY, 
                                                bool (*isWallFunc)(int, int)) {
    const int WIDTH = 120;
    const int HEIGHT = 43;
    
    if (startX < 0 || startX >= WIDTH || startY < 0 || startY >= HEIGHT ||
        targetX < 0 || targetX >= WIDTH || targetY < 0 || targetY >= HEIGHT) {
        return {};
    }
    
    std::vector<std::vector<int>> dist(HEIGHT, std::vector<int>(WIDTH, INT_MAX));
    std::vector<std::vector<std::pair<int, int>>> prev(HEIGHT, std::vector<std::pair<int, int>>(WIDTH, {-1, -1}));
    std::vector<std::vector<bool>> visited(HEIGHT, std::vector<bool>(WIDTH, false));
    
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> pq;
    
    dist[startY][startX] = 0;
    pq.push({startX, startY, 0});
    
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    
    while (!pq.empty()) {
        PathNode current = pq.top();
        pq.pop();
        
        int x = current.x;
        int y = current.y;
        int cost = current.cost;
        
        if (visited[y][x]) continue;
        visited[y][x] = true;

        if (x == targetX && y == targetY) {
            break;
        }
        
        for (int i = 0; i < 4; ++i) {
            int newX = x + dx[i];
            int newY = y + dy[i];

            if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) continue;
            

            if (visited[newY][newX]) continue;
            
            if (!canStandAt(newX, newY, isWallFunc)) continue;
            
            int newCost = cost + 1;
            if (newCost < dist[newY][newX]) {
                dist[newY][newX] = newCost;
                prev[newY][newX] = {x, y};
                pq.push({newX, newY, newCost});
            }
        }
    }
    
    std::vector<std::pair<int, int>> path;
    int currentX = targetX, currentY = targetY;
    
    if (dist[currentY][currentX] == INT_MAX) {
        return {};
    }
    
    while (currentX != -1 && currentY != -1) {
        path.push_back({currentX, currentY});
        int nextX = prev[currentY][currentX].first;
        int nextY = prev[currentY][currentX].second;
        currentX = nextX;
        currentY = nextY;
    }
    
    std::reverse(path.begin(), path.end());
    
    if (!path.empty() && path[0].first == startX && path[0].second == startY) {
        path.erase(path.begin());
    }
    
    return path;
}


Enemy::Enemy(EnemyType t, int startX, int startY) 
    : type(t), x(startX), y(startY), 
      targetX(startX), targetY(startY),
      spawnTime(std::chrono::steady_clock::now()),
      waitDuration(5 + rand() % 6),
      targetDoorId(-1),
      lastTargetUpdateTime(std::chrono::steady_clock::now()) {
    
    switch (t) {
        case EnemyType::FREDDY:
            spawnDelaySeconds = 0;
            moveIntervalMs = 400;
            break;
        case EnemyType::CHICA:
            spawnDelaySeconds = 0;
            moveIntervalMs = 500;
            break;
        case EnemyType::BONNIE:
            spawnDelaySeconds = 0;
            moveIntervalMs = 300;
            break;
    }
    
    findNearestFreePosition(startX, startY, nullptr);

}

int Enemy::getCurrentZone() const {
    for (const auto& area : AREAS) {
        if (area.contains(x, y)) {
            return area.id;
        }
    }
    return -1;
}

bool Enemy::shouldChasePlayer(int playerX, int playerY, bool playerInOffice) const {
    if (playerInOffice) {
        return false;
    }
    
    bool enemyInOffice = (x >= 44 && x <= 67 && y >= 33 && y <= 41);
    if (enemyInOffice) {
        return false;
    }
    
    int distX = std::abs(x - playerX);
    int distY = std::abs(y - playerY);
    return distX <= 10 && distY <= 10;
}

void Enemy::findNearestFreePosition(int targetX, int targetY, bool (*isWallFunc)(int, int)) {
    if (!isWallFunc) return;
    
    if (canStandAt(targetX, targetY, isWallFunc)) {
        x = targetX;
        y = targetY;
        return;
    }

    for (int radius = 1; radius <= 5; ++radius) {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                if (std::abs(dx) == radius || std::abs(dy) == radius) {
                    int nx = targetX + dx;
                    int ny = targetY + dy;
                    if (canStandAt(nx, ny, isWallFunc)) {
                        x = nx;
                        y = ny;
                        return;
                    }
                }
            }
        }
    }
}

const Camera* Enemy::getCurrentCamera() const {
    int currentZone = getCurrentZone();
    
    if (currentZone == -1) return nullptr;
    
    if (currentZone == THE_OFFICE) {
        for (const auto& cam : CAMERAS) {
            if (cam.id == CAM_OFFICE) {
                return &cam;
            }
        }
    }
    
    if (currentZone == SHOW_STAGE) {
        switch (type) {
            case EnemyType::CHICA:
                return getCameraById(CAM_1A_CHICA);
            case EnemyType::FREDDY:
                return getCameraById(CAM_1A_FREDDY);
            case EnemyType::BONNIE:
                return getCameraById(CAM_1A_BONNIE);
        }
    }
    
    const Camera* bestCam = nullptr;
    int bestDist = 1000000;
    
    for (const auto& cam : CAMERAS) {
        if (cam.id == CAM_1A_CHICA || cam.id == CAM_1A_FREDDY || 
            cam.id == CAM_1A_BONNIE || cam.id == CAM_OFFICE) {
            continue;
        }
        
        bool cameraInSameZone = false;
        for (const auto& area : AREAS) {
            if (area.id == currentZone && area.contains(cam.x, cam.y)) {
                cameraInSameZone = true;
                break;
            }
        }
        
        if (cameraInSameZone) {
            int dist = std::abs(x - cam.x) + std::abs(y - cam.y);
            if (dist < bestDist) {
                bestDist = dist;
                bestCam = &cam;
            }
        }
    }
    
    return bestCam;
}

std::map<int, std::vector<int>> ZONE_TO_CAMERAS = {
    {SHOW_STAGE, {CAM_1A_CHICA, CAM_1A_FREDDY, CAM_1A_BONNIE}},
    {DINING_AREA, {CAM_1B}},
    {RESTROOMS, {CAM_7}},
    {KITCHEN, {CAM_6}},
    {EAST_HALL, {CAM_4A, CAM_4B}},
    {WEST_HALL, {CAM_2A, CAM_2B}},
    {BACKSTAGE, {CAM_5}},
    {SUPPLY_CLOSET, {CAM_3}},
    {THE_OFFICE, {CAM_OFFICE}}
};

bool Enemy::canStandAt(int x, int y, bool (*isWallFunc)(int, int)) const {
    return canEnemyMoveTo(x, y, type, isWallFunc);
}
void Enemy::moveToNextCamera() {
    const Camera* currentCam = getCurrentCamera();
    if (!currentCam) return;
    
    const std::vector<CameraRoute>* routes = nullptr;
    switch (type) {
        case EnemyType::FREDDY: routes = &FREDDY_ROUTES; break;
        case EnemyType::CHICA: routes = &CHICA_ROUTES; break;
        case EnemyType::BONNIE: routes = &BONNIE_ROUTES; break;
        default: return;
    }
    
    const CameraRoute* currentRoute = nullptr;
    for (const auto& route : *routes) {
        if (route.currentCamera == currentCam->id) {
            currentRoute = &route;
            break;
        }
    }
    
    if (!currentRoute || currentRoute->nextOptions.empty()) {
        return;
    }
    
    size_t choice = rand() % currentRoute->nextOptions.size();
    int doorId = currentRoute->nextOptions[choice].first;
    int nextCameraId = currentRoute->nextOptions[choice].second;
    
    if (nextCameraId == CAM_OFFICE) {
        bool officeDoorOpen = true;
        if (type == EnemyType::BONNIE) {
            extern bool leftDoorOpen;
            officeDoorOpen = leftDoorOpen;
        } else {
            extern bool rightDoorOpen;
            officeDoorOpen = rightDoorOpen;
        }
        
        if (!officeDoorOpen) {
            bool foundAlternative = false;
            for (size_t i = 0; i < currentRoute->nextOptions.size(); ++i) {
                if (currentRoute->nextOptions[i].second != CAM_OFFICE) {
                    choice = i;
                    doorId = currentRoute->nextOptions[choice].first;
                    nextCameraId = currentRoute->nextOptions[choice].second;
                    foundAlternative = true;
                    break;
                }
            }
            if (!foundAlternative) {
                return;
            }
        }
    }

    if (doorId > 0) {
        const Door* door = getDoorById(doorId);
        if (door) {
            int doorX = door->getCenterX();
            int doorY = door->getCenterY();
            
            currentPath = findPath(x, y, doorX, doorY, [](int x, int y) {
                return Game::isWallAtStatic(x, y);
            });
            currentPathIndex = 0;

            targetX = doorX;
            targetY = doorY;
            state = EnemyState::MOVING_THROUGH_DOOR;
            nextCameraAfterDoor = nextCameraId;
            lastTargetUpdateTime = std::chrono::steady_clock::now();
        }
    } else if (nextCameraId > 0) {
        const Camera* nextCam = getCameraById(nextCameraId);
        if (nextCam) {
            currentPath = findPath(x, y, nextCam->x, nextCam->y, [](int x, int y) {
                return Game::isWallAtStatic(x, y);
            });
            currentPathIndex = 0;

            targetX = nextCam->x;
            targetY = nextCam->y;
            state = EnemyState::MOVING_TO_CAMERA;
            lastTargetUpdateTime = std::chrono::steady_clock::now();
        }
    }
}

void Enemy::followPath(bool (*isWallFunc)(int, int)) {
    if (!isWallFunc) return;

    if (currentPathIndex < currentPath.size()) {
        int nextX = currentPath[currentPathIndex].first;
        int nextY = currentPath[currentPathIndex].second;
        
        if (x < nextX && canStandAt(x + 1, y, isWallFunc)) {
            x++;
        } else if (x > nextX && canStandAt(x - 1, y, isWallFunc)) {
            x--;
        } else if (y < nextY && canStandAt(x, y + 1, isWallFunc)) {
            y++;
        } else if (y > nextY && canStandAt(x, y - 1, isWallFunc)) {
            y--;
        } else {
            currentPath.clear();
            currentPathIndex = 0;
            return;
        }
        
        if (x == nextX && y == nextY) {
            currentPathIndex++;
        }
        return;
    }
    
    int distX = std::abs(targetX - x);
    int distY = std::abs(targetY - y);
    
    if (distX >= distY) {
        if (x < targetX && canStandAt(x + 1, y, isWallFunc)) x++;
        else if (x > targetX && canStandAt(x - 1, y, isWallFunc)) x--;
        else if (y < targetY && canStandAt(x, y + 1, isWallFunc)) y++;
        else if (y > targetY && canStandAt(x, y - 1, isWallFunc)) y--;
    } else {
        if (y < targetY && canStandAt(x, y + 1, isWallFunc)) y++;
        else if (y > targetY && canStandAt(x, y - 1, isWallFunc)) y--;
        else if (x < targetX && canStandAt(x + 1, y, isWallFunc)) x++;
        else if (x > targetX && canStandAt(x - 1, y, isWallFunc)) x--;
    }
}

void Enemy::update(bool officeDoorOpen, int playerX, int playerY, bool playerInOffice,
                  int doorX, int doorY, bool (*isWallFunc)(int, int)) {
    
    if (!isWallFunc) return;
    
    auto now = std::chrono::steady_clock::now();
    
    auto stuckTime = std::chrono::duration_cast<std::chrono::seconds>(now - lastTargetUpdateTime);
    if (stuckTime.count() >= 10) {
        x = targetX;
        y = targetY;
        lastTargetUpdateTime = now;
        if (state == EnemyState::MOVING_TO_CAMERA || state == EnemyState::MOVING_THROUGH_DOOR) {
            state = EnemyState::WAITING;
            waitTime = now;
            waitDuration = 2 + rand() % 3;
        }
    }
    
    if (shouldChasePlayer(playerX, playerY, playerInOffice)) {
        targetX = playerX;
        targetY = playerY;
        state = EnemyState::MOVING_TO_PLAYER;
        followPath(isWallFunc);
        return;
    }
    
    if (state == EnemyState::SPAWNING) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - spawnTime);
        if (elapsed.count() >= spawnDelaySeconds) {
            state = EnemyState::WAITING;
            waitTime = now;
            waitDuration = 5 + rand() % 6;
        }
        return;
    }
    
    if (state == EnemyState::WAITING) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - waitTime);
        if (elapsed.count() >= waitDuration) {
            moveToNextCamera();
        }
        return;
    }
    
    if (state == EnemyState::MOVING_TO_CAMERA || state == EnemyState::MOVING_THROUGH_DOOR) {
        if (std::abs(x - targetX) <= 1 && std::abs(y - targetY) <= 1) {
            state = EnemyState::WAITING;
            waitTime = now;
            waitDuration = 5 + rand() % 6;
            lastTargetUpdateTime = now;
        } else {
            followPath(isWallFunc);
        }
        return;
    }
    
    if (state == EnemyState::MOVING_TO_PLAYER) {
        if (std::abs(x - targetX) <= 1 && std::abs(y - targetY) <= 1) {
            state = EnemyState::WAITING;
            waitTime = now;
            waitDuration = 1;
            lastTargetUpdateTime = now;
        } else {
            followPath(isWallFunc);
        }
        return;
    }
}