#include "room.h"

Room::Room(Floor* floor, u_char x, u_char y, u_char width, u_char height)
        : Terrain(floor, x, y) {
    this->width = width;
    this->height = height;

    this->hardness = ROOM_HARDNESS;
    this->character = ROOM_CHARACTER;
    this->type = TERRAIN_ROOM;
}

u_char Room::randomXInside() {
    return Dice::RandomNumberBetween(this->getStartX(), this->getEndX());
}

u_char Room::randomYInside() {
    return Dice::RandomNumberBetween(this->getStartY(), this->getEndY());
}

u_char Room::getStartX() {
    return this->x;
}

u_char Room::getStartY() {
    return this->y;
}

u_char Room::getEndX() {
    return this->x + this->width - 1;
}

u_char Room::getEndY() {
    return this->y + this->height - 1;
}

u_char Room::getArea() {
    return this->width * this->height;
}

/** GETTERS **/
u_char Room::getWidth() {
    return this->width;
}

u_char Room::getHeight() {
    return this->height;
}

u_char Room::getStartingX() {
    return this->x;
}

u_char Room::getStartingY() {
    return this->y;
}
/** GETTERS **/

/** SETTERS **/
/** SETTERS */