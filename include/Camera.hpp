#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <vector>
#include <string>

struct Door {
    int id;
    bool isHorizontal;
    int x1, x2, y1, y2;
    std::string name;
    
    struct HorizontalTag {};
    Door(int id, int zone1, int zone2, int x1, int x2, int y, const std::string& name, HorizontalTag)
        : id(id), isHorizontal(true), x1(x1), x2(x2), y1(y), y2(y), name(name) {}
    
    struct VerticalTag {};
    Door(int id, int zone1, int zone2, int x, int y1, int y2, const std::string& name, VerticalTag)
        : id(id), isHorizontal(false), x1(x), x2(x), y1(y1), y2(y2), name(name) {}
    
    bool contains(int x, int y) const {
        if (isHorizontal) {
            return y == y1 && x >= x1 && x <= x2;
        } else {
            return x == x1 && y >= y1 && y <= y2;
        }
    }
    
    int getCenterX() const { 
        return isHorizontal ? (x1 + x2) / 2 : x1; 
    }
    
    int getCenterY() const { 
        return isHorizontal ? y1 : (y1 + y2) / 2; 
    }
};

struct Camera {
    int id;
    int x, y;
    std::string name;
    Camera(int id, int x, int y, const std::string& name) 
        : id(id), x(x), y(y), name(name) {}
};



extern const std::vector<Camera> CAMERAS;
extern const std::vector<Door> DOORS;

int getCameraIndexById(int id);
const Camera* getCameraById(int id);
const Door* getDoorById(int id);
const int CAM_OFFICE = 23;

#endif // CAMERA_HPP