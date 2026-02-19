#include "Camera.hpp"
#include "Areas.hpp" 

const std::vector<Camera> CAMERAS = {
    Camera(CAM_1A_CHICA, 66, 2, "1A"),
    Camera(CAM_1A_FREDDY, 56, 2, "1A"),
    Camera(CAM_1A_BONNIE, 46, 2, "1A"),
    Camera(CAM_1B, 27, 11, "1B"),
    Camera(CAM_5, 13, 7, "5"),
    Camera(CAM_2A, 37, 25, "2A"),
    Camera(CAM_2B, 37, 40, "2B"),
    Camera(CAM_4A, 73, 25, "4A"),
    Camera(CAM_4B, 73, 40, "4B"),
    Camera(CAM_3, 21, 34, "3"),
    Camera(CAM_7, 99, 13, "7"),
    Camera(CAM_6, 93, 28, "6"),
    Camera(CAM_OFFICE, 55, 37, "OFFICE")
};
bool leftDoorOpen = true;
bool rightDoorOpen = true;

const std::vector<Door> DOORS = {
    Door(DOOR_SHOW_DINING, SHOW_STAGE, DINING_AREA, 36, 75, 5, "DOOR_SHOW_DINING", Door::HorizontalTag{}),
    Door(DOOR_DINING_WEST, DINING_AREA, WEST_HALL, 31, 43, 23, "DOOR_DINING_WEST", Door::HorizontalTag{}),
    Door(DOOR_DINING_EAST, DINING_AREA, EAST_HALL, 68, 80, 23, "DOOR_DINING_EAST", Door::HorizontalTag{}),
    Door(DOOR_RESTROOMS_DINING, RESTROOMS, DINING_AREA, 92, 12, 15, "DOOR_RESTROOMS_DINING", Door::VerticalTag{}),
    Door(DOOR_KITCHEN_DINING, KITCHEN, DINING_AREA, 84, 90, 23, "DOOR_KITCHEN_DINING", Door::HorizontalTag{}),
    Door(DOOR_SUPPLY_WEST, SUPPLY_CLOSET, WEST_HALL, 31, 31, 34, "DOOR_SUPPLY_WEST", Door::VerticalTag{}),
    Door(DOOR_BACKSTAGE_DINING, BACKSTAGE, DINING_AREA, 20, 7, 10, "DOOR_BACKSTAGE_DINING", Door::VerticalTag{}),

    Door(DOOR_OFFICE_LEFT, WEST_HALL, THE_OFFICE, 44, 40, 33, "DOOR_OFFICE_LEFT", Door::VerticalTag{}),
    Door(DOOR_OFFICE_RIGHT, EAST_HALL, THE_OFFICE, 66, 40, 33, "DOOR_OFFICE_RIGHT", Door::VerticalTag{})
};



int getCameraIndexById(int id) {
    for (size_t i = 0; i < CAMERAS.size(); ++i) {
        if (CAMERAS[i].id == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

const Camera* getCameraById(int id) {
    int idx = getCameraIndexById(id);
    return (idx >= 0) ? &CAMERAS[idx] : nullptr;
}

const Door* getDoorById(int id) {
    for (const auto& door : DOORS) {
        if (door.id == id) {
            return &door;
        }
    }
    return nullptr;
}