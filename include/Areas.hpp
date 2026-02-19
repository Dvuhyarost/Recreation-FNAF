#ifndef AREAS_HPP
#define AREAS_HPP

#include <vector>
#include <string>

// Area IDs
enum AreaID {
    SHOW_STAGE = 0,
    DINING_AREA = 1,
    PIRATE_COVE = 2,
    RESTROOMS = 3,
    EAST_HALL = 4,
    WEST_HALL = 5,
    KITCHEN = 6,
    BACKSTAGE = 7,
    SUPPLY_CLOSET = 8,
    THE_OFFICE = 9
};

// Camera IDs
enum CameraID {
    CAM_1A_CHICA = 10,
    CAM_1A_FREDDY = 11,
    CAM_1A_BONNIE = 12,
    CAM_1B = 13,
    CAM_5 = 14,
    CAM_2A = 16,
    CAM_2B = 18,
    CAM_4A = 17,
    CAM_4B = 19,
    CAM_3 = 20,
    CAM_7 = 21,
    CAM_6 = 22
};

// Door IDs
enum DoorID {
    DOOR_SHOW_DINING = 100,
    DOOR_DINING_WEST = 101,
    DOOR_DINING_EAST = 102,
    DOOR_RESTROOMS_DINING = 103,
    DOOR_KITCHEN_DINING = 104,
    DOOR_SUPPLY_WEST = 105,
    DOOR_BACKSTAGE_DINING = 108,
    DOOR_OFFICE_LEFT = 106,
    DOOR_OFFICE_RIGHT = 107
};

struct Area {
    int id;
    std::string name;
    int x1, x2, y1, y2;
    const char* color;
    
    bool contains(int x, int y) const {
        return x >= x1 && x <= x2 && y >= y1 && y <= y2;
    }
};

extern const std::vector<Area> AREAS;

#endif // AREAS_HPP
