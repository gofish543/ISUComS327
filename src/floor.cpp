#include "floor.h"

Floor::Floor(Dungeon* dungeon, u_char floorNumber, u_short roomCount, u_short stairUpCount, u_short stairDownCount, u_short numberOfMonsters) {
    this->dungeon = dungeon;
    this->floorNumber = floorNumber;

    this->roomCount = roomCount;
    this->stairUpCount = stairUpCount;
    this->stairDownCount = stairDownCount;

    if (this->floorNumber == 0) {
        this->stairDownCount = 0;
    }
    if (this->floorNumber == this->dungeon->getFloorCount() - 1) {
        this->stairUpCount = 0;
    }

    if (this->dungeon->getSettings()->doNumberOfMonsters()) {
        this->monsterCount = this->dungeon->getSettings()->getNumberOfMonsters();
    } else {
        this->monsterCount = numberOfMonsters;
    }

    this->initializeToNull()
            ->generateBorders()
            ->generateRooms()
            ->generateCorridors()
            ->generateRock()
            ->generatePlayer()
            ->generateMonsters();
}

Floor::~Floor() {
    u_short index;
    u_char height;
    u_char width;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            delete (this->terrains[height][width]);
        }
    }

    for (index = 0; index < this->roomCount; index++) {
        delete (this->rooms.at(index));
    }

    for (index = 0; index < this->stairUpCount; index++) {
        delete (this->upStairs.at(index));
    }

    for (index = 0; index < this->stairDownCount; index++) {
        delete (this->downStairs.at(index));
    }

    for (index = 0; index < this->monsterCount; index++) {
        delete (this->monsters.at(index));
    }

    if(this->getDungeon()->getPlayer() != null) {
        delete(this->getDungeon()->getPlayer());
        this->getDungeon()->setPlayer(null);
    }
}

Floor* Floor::resetTunnelerView() {
    u_char height;
    u_char width;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            this->tunnelerView[height][width] = U_CHAR_MIN;
        }
    }

    return this;
}

Floor* Floor::resetNonTunnelerView() {
    u_char height;
    u_char width;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            this->nonTunnelerView[height][width] = U_CHAR_MIN;
        }
    }

    return this;
}

Floor* Floor::resetCheapestPathToPlayer() {
    u_char height;
    u_char width;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            this->cheapestPathToPlayer[height][width] = U_CHAR_MIN;
        }
    }

    return this;
}

u_char Floor::getPrintCharacterAt(u_char width, u_char height) {
    if (this->characters[height][width] == null) {
        return this->terrains[height][width]->getCharacter();
    } else {
        return this->characters[height][width]->getCharacter();
    }
}

/** GETTERS **/
Dungeon* Floor::getDungeon() {
    return this->dungeon;
}

u_char Floor::getFloorNumber() {
    return this->floorNumber;
}

u_short Floor::getRoomCount() {
    return this->roomCount;
}

u_short Floor::getStairUpCount() {
    return this->stairUpCount;
}

u_short Floor::getStairDownCount() {
    return this->stairDownCount;
}

u_short Floor::getMonsterCount() {
    return this->monsterCount;
}

Terrain* Floor::getTerrainAt(u_char width, u_char height) {
    return this->terrains[height][width];
}

Character* Floor::getCharacterAt(u_char width, u_char height) {
    return this->characters[height][width];
}

u_char Floor::getTunnelerViewAt(u_char width, u_char height) {
    return this->tunnelerView[height][width];
}

u_char Floor::getNonTunnelerViewAt(u_char width, u_char height) {
    return this->nonTunnelerView[height][width];
}

u_char Floor::getCheapestPathToPlayerAt(u_char width, u_char height) {
    return this->cheapestPathToPlayer[height][width];
}

std::vector<Monster*> Floor::getMonsters() {
    return this->monsters;
}

std::vector<Staircase*> Floor::getUpStairs() {
    return this->upStairs;
}

std::vector<Staircase*> Floor::getDownStairs() {
    return this->downStairs;
}

std::vector<Room*> Floor::getRooms() {
    return this->rooms;
}
/** GETTERS **/

/** SETTERS **/
Floor* Floor::setDungeon(Dungeon* dungeon) {
    this->dungeon = dungeon;

    return this;
}

Floor* Floor::setFloorNumber(u_char floorNumber) {
    this->floorNumber = floorNumber;

    return this;
}

Floor* Floor::setRoomCount(u_short roomCount) {
    this->roomCount = roomCount;

    return this;
}

Floor* Floor::setStairUpCount(u_short stairUpCount) {
    this->stairUpCount = stairUpCount;

    return this;
}

Floor* Floor::setStairDownCount(u_short stairDownCount) {
    this->stairDownCount = stairDownCount;

    return this;
}

Floor* Floor::setMonsterCount(u_short monsterCount) {
    this->monsterCount = monsterCount;

    return this;
}

Floor* Floor::setTerrainAt(Terrain* terrain, u_char width, u_char height) {
    this->terrains[height][width] = terrain;

    return this;
}

Floor* Floor::setCharacterAt(Character* character, u_char width, u_char height) {
    this->characters[height][width] = character;

    return this;
}

Floor* Floor::setTunnelerViewAt(u_char value, u_char width, u_char height) {
    this->tunnelerView[height][width] = value;

    return this;
}

Floor* Floor::setNonTunnelerViewAt(u_char value, u_char width, u_char height) {
    this->nonTunnelerView[height][width] = value;

    return this;
}

Floor* Floor::setCheapestPathToPlayer(u_char value, u_char width, u_char height) {
    this->cheapestPathToPlayer[height][width] = value;

    return this;
}

Floor* Floor::setMonsters(std::vector<Monster*>& monsters) {
    this->monsters = monsters;

    return this;
}

Floor* Floor::setUpStairs(std::vector<Staircase*>& upStairs) {
    this->upStairs = upStairs;

    return this;
}

Floor* Floor::setDownStairs(std::vector<Staircase*>& downStairs) {
    this->downStairs = downStairs;

    return this;
}

Floor* Floor::setRooms(std::vector<Room*>& rooms) {
    this->rooms = rooms;

    return this;
}
/** SETTERS **/

Floor* Floor::initializeToNull() {
    u_char height;
    u_char width;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            this->terrains[height][width] = null;
            this->tunnelerView[height][width] = U_CHAR_MIN;
            this->nonTunnelerView[height][width] = U_CHAR_MIN;
            this->cheapestPathToPlayer[height][width] = U_CHAR_MIN;
        }
    }

    return this;
}

Floor* Floor::generateBorders() {
    u_char height;
    u_char width;

    // Set NORTH and SOUTH walls
    for (width = 1; width < DUNGEON_FLOOR_WIDTH - 1; width++) {
        // Create a new border
        this->terrains[0][width] = new Border(this, 0, 0, width, NORTH_SOUTH_WALL_CHARACTER);
        this->terrains[DUNGEON_FLOOR_HEIGHT - 1][width] = new Border(this, 0, width, DUNGEON_FLOOR_HEIGHT - 1, NORTH_SOUTH_WALL_CHARACTER);
    }

    // Set EAST and WEST walls
    for (height = 1; height < DUNGEON_FLOOR_HEIGHT - 1; height++) {
        // Create a new border
        this->terrains[height][0] = new Border(this, 0, 0, height, EAST_WEST_WALL_CHARACTER);
        this->terrains[height][DUNGEON_FLOOR_WIDTH - 1] = new Border(this, 0, DUNGEON_FLOOR_WIDTH - 1, height, EAST_WEST_WALL_CHARACTER);
    }

    // Set top left corner
    this->terrains[0][0] = new Border(this, 0, 0, 0, CORNER_WALL_CHARACTER);

    // Set top right corner
    this->terrains[0][DUNGEON_FLOOR_WIDTH - 1] = new Border(this, 0, DUNGEON_FLOOR_WIDTH - 1, 0, CORNER_WALL_CHARACTER);

    // Set bottom left corner
    this->terrains[DUNGEON_FLOOR_HEIGHT - 1][0] = new Border(this, 0, 0, DUNGEON_FLOOR_HEIGHT - 1, CORNER_WALL_CHARACTER);

    // Set bottom right corner
    this->terrains[DUNGEON_FLOOR_HEIGHT - 1][DUNGEON_FLOOR_WIDTH - 1] = new Border(this, 0, DUNGEON_FLOOR_WIDTH - 1, DUNGEON_FLOOR_HEIGHT - 1, CORNER_WALL_CHARACTER);

    return this;
}

Floor* Floor::generateRock() {
    u_char height;
    u_char width;
    u_char index;
    u_char x;
    u_char y;
    u_int s;
    u_int t;
    u_int p;
    u_int q;

    int gaussian[5][5] = {
            {1, 4,  7,  4,  1},
            {4, 16, 26, 16, 4},
            {7, 26, 41, 26, 7},
            {4, 16, 26, 16, 4},
            {1, 4,  7,  4,  1}
    };

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            if (this->terrains[height][width] == null) {
                this->terrains[height][width] = new Rock(this, 0, width, height, u_char(random_number_between(ROCK_HARDNESS_MIN, ROCK_HARDNESS_MAX)));
            }
        }
    }

    for (index = 0; index < 3; index++) {
        for (y = 0; y < DUNGEON_FLOOR_HEIGHT; y++) {
            for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
                for (s = t = p = 0; p < 5; p++) {
                    for (q = 0; q < 5; q++) {
                        if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_FLOOR_HEIGHT &&
                            x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_FLOOR_WIDTH) {
                            s += gaussian[p][q];
                            t += this->getTerrainAt(u_char(x + (q - 2)), u_char(y + (p - 2)))->getHardness() * gaussian[p][q];
                        }
                    }
                }
                if (this->terrains[y][x] != null && this->terrains[y][x]->isRock()) {
                    this->getTerrainAt(x, y)->setHardness(u_char(t / s));
                }
            }
        }
    }

    return this;
}

Floor* Floor::generateRooms() {
    // Do some quick math to determine if the floor has room for all these rooms
    if (this->roomCount * (ROOM_MAX_WIDTH * ROOM_MAX_HEIGHT) > .6 * (DUNGEON_FLOOR_WIDTH * DUNGEON_FLOOR_HEIGHT)) {
        // If the max room characters is greater than 50% of the space available in the room, leave
        std::cout << "Cannot accurately place rooms with the given settings, more than 60% of the dungeon could be rooms" << std::endl;
    }
    u_short index;
    u_short roomIndex;

    u_char roomStartX;
    u_char roomStartY;

    u_char roomWidth;
    u_char roomHeight;

    u_char width;
    u_char height;

    bool collision;

    // Place to store the random stair room ids
    u_char stairX;
    u_char stairY;

    u_short upStairRooms[this->stairUpCount];
    u_short downStairRooms[this->stairDownCount];

    // Determine the rooms for each of the staircases
    for (roomIndex = 0; roomIndex < this->stairUpCount; roomIndex++) {
        upStairRooms[roomIndex] = u_short(random_number_between(0, this->roomCount - 1));
    }

    for (roomIndex = 0; roomIndex < this->stairDownCount; roomIndex++) {
        downStairRooms[roomIndex] = u_short(random_number_between(0, this->roomCount - 1));
    }

    // Place each room
    for (index = 0; index < this->roomCount; index++) {
        do {
            collision = false;

            // Find something inside the game play box
            roomStartX = u_char(random_number_between(1, DUNGEON_FLOOR_WIDTH - 2));
            roomStartY = u_char(random_number_between(1, DUNGEON_FLOOR_HEIGHT - 2));

            // Select a random height and width
            roomWidth = u_char(random_number_between(ROOM_MIN_WIDTH, ROOM_MAX_WIDTH));
            roomHeight = u_char(random_number_between(ROOM_MIN_HEIGHT, ROOM_MAX_HEIGHT));

            // Need to check boundaries one off to make sure they are open spaces
            for (height = roomStartY - u_char(1); height < (roomStartY + roomHeight + 1); height++) {
                // Need to check boundaries one off to make sure they are open spaces
                for (width = roomStartX - u_char(1); width < (roomStartX + roomWidth + 1); width++) {
                    if (
                            width >= DUNGEON_FLOOR_WIDTH - 1 || // Check if width in bounds
                            height >= DUNGEON_FLOOR_HEIGHT - 1 || // Check if height in bounds
                            this->terrains[height][width] != null // Check if another item already exists
                            ) {
                        collision = true;
                    }
                }
            }
        } while (collision);

        this->rooms.push_back(new Room(this, index, roomStartX, roomStartY, roomStartX, roomStartY, roomWidth, roomHeight));

        // Are we placing any staircases in this room?
        for (roomIndex = 0; roomIndex < this->stairUpCount; roomIndex++) {
            if (upStairRooms[roomIndex] == index) {
                // Place the up staircase
                stairX = u_char(random_number_between(roomStartX + 1, roomStartX + roomWidth - 2));
                stairY = u_char(random_number_between(roomStartY + 1, roomStartY + roomHeight - 2));

                this->terrains[stairY][stairX] = new Staircase(this, index, stairX, stairY, STAIRCASE_TYPE_UP);
                this->upStairs.push_back(new Staircase(this, index, stairX, stairY, STAIRCASE_TYPE_UP));
            }
        }

        for (roomIndex = 0; roomIndex < this->stairDownCount; roomIndex++) {
            if (downStairRooms[roomIndex] == index) {
                // Place the down staircase
                stairX = u_char(random_number_between(roomStartX + 1, roomStartX + roomWidth - 2));
                stairY = u_char(random_number_between(roomStartY + 1, roomStartY + roomHeight - 2));

                this->terrains[stairY][stairX] = new Staircase(this, index, stairX, stairY, STAIRCASE_TYPE_DOWN);
                this->downStairs.push_back(new Staircase(this, index, stairX, stairY, STAIRCASE_TYPE_DOWN));
            }
        }

        // Valid room finally found, place it
        for (height = roomStartY; height < roomStartY + roomHeight; height++) {
            for (width = roomStartX; width < roomStartX + roomWidth; width++) {
                if (this->terrains[height][width] == null) {
                    this->terrains[height][width] = new Room(this, index, width, height, roomStartX, roomStartY, roomWidth, roomHeight);
                }
            }
        }
    }

    return this;
}

Floor* Floor::generateCorridors() {
    bool upValid;
    bool downValid;
    bool leftValid;
    bool rightValid;

    u_char firstRoomX;
    u_char firstRoomY;

    u_char secondRoomX;
    u_char secondRoomY;

    u_char tempX;
    u_char tempY;

    u_short index;
    for (index = 0; index < this->roomCount - 1; index++) {
        // First we want to select a random spot within the room, but it needs to be on the border
        do {
            firstRoomX = u_char(random_number_between(this->rooms[index]->getStartingX(), this->rooms[index]->getStartingX() + this->rooms[index]->getWidth() - 1));
            firstRoomY = u_char(random_number_between(this->rooms[index]->getStartingY(), this->rooms[index]->getStartingY() + this->rooms[index]->getHeight() - 1));

            upValid = this->terrains[firstRoomY - 1][firstRoomX] == null;
            downValid = this->terrains[firstRoomY + 1][firstRoomX] == null;
            leftValid = this->terrains[firstRoomY][firstRoomX - 1] == null;
            rightValid = this->terrains[firstRoomY][firstRoomX + 1] == null;
        } while (upValid || downValid || leftValid || rightValid);

        // Second we want to select a random spot within the next room, but it needs to be on the border
        do {
            secondRoomX = u_char(random_number_between(this->rooms[index + 1]->getStartingX(), this->rooms[index + 1]->getStartingX() + this->rooms[index + 1]->getWidth() - 1));
            secondRoomY = u_char(random_number_between(this->rooms[index + 1]->getStartingY(), this->rooms[index + 1]->getStartingY() + this->rooms[index + 1]->getHeight() - 1));

            upValid = this->terrains[secondRoomY - 1][secondRoomX] == null;
            downValid = this->terrains[secondRoomY + 1][secondRoomX] == null;
            leftValid = this->terrains[secondRoomY][secondRoomX - 1] == null;
            rightValid = this->terrains[secondRoomY][secondRoomX + 1] == null;
        } while (upValid || downValid || leftValid || rightValid);

        // Now connect them in the X direction
        tempX = firstRoomX;
        while (tempX != secondRoomX) {
            if (tempX > secondRoomX) {
                tempX--;
            } else if (tempX < secondRoomX) {
                tempX++;
            }

            if (this->terrains[secondRoomY][tempX] == null) {
                this->terrains[secondRoomY][tempX] = new Corridor(this, 0, tempX, secondRoomY);
            }
        }

        // Now connect them in the Y direction
        tempY = secondRoomY;
        while (tempY != firstRoomY) {
            if (tempY > firstRoomY) {
                tempY--;
            } else if (tempY < firstRoomY) {
                tempY++;
            }

            if (this->terrains[tempY][firstRoomX] == null) {
                this->terrains[tempY][firstRoomX] = new Corridor(this, 0, firstRoomX, tempY);
            }
        }

        // Handle the corner case
        if (this->terrains[secondRoomY][firstRoomX] == null) {
            this->terrains[secondRoomY][firstRoomX] = new Corridor(this, 0, firstRoomX, secondRoomY);
        }
    }

    return this;
}

Floor* Floor::generatePlayer() {
    if (this->floorNumber == 0 && this->getDungeon()->getPlayer() == null) {
        // Need to place player
        // Select random room and random coords and place character there
        u_char playerX;
        u_char playerY;
        auto room = u_short(random_number_between(0, this->roomCount - 1));
        do {
            playerX = u_char(random_number_between(this->rooms.at(room)->getStartingX(), this->rooms.at(room)->getStartingX() + this->rooms.at(room)->getWidth() - 1));
            playerY = u_char(random_number_between(this->rooms.at(room)->getStartingY(), this->rooms.at(room)->getStartingY() + this->rooms.at(room)->getHeight() - 1));
        } while (this->getTerrainAt(playerX, playerY)->getCharacter() == STAIRCASE_UP_CHARACTER && this->getTerrainAt(playerX, playerY)->getCharacter() == STAIRCASE_DOWN_CHARACTER);

        dungeon->setPlayer(new Player(this, playerX, playerY));
        this->setCharacterAt(dungeon->getPlayer(), playerX, playerY);
    }

    return this;
}

Floor* Floor::generateMonsters() {
    u_char width;
    u_char height;

    u_short index;

    u_char monsterX;
    u_char monsterY;
    u_short monsterRoom;

    u_char classification;
    u_char speed;

    auto playerRoom = (Room*) this->getTerrainAt(this->dungeon->getPlayer()->getX(), this->dungeon->getPlayer()->getY());

    u_int placementAttempts = 0;
    u_short maxMonsters = 0;
    u_char roomIndex;

    // Find out the max number of monsters possible within the program
    for (roomIndex = 0; roomIndex < this->roomCount; roomIndex++) {
        if (this->rooms.at(roomIndex)->getId() == playerRoom->getId() && this->rooms.at(roomIndex)->getFloor() == dungeon->getPlayer()->getFloor()) {
            continue;
        } else {
            maxMonsters += (this->rooms.at(roomIndex)->getWidth() * this->rooms.at(roomIndex)->getHeight());
        }
    }

    // If there are more monsters trying to be placed than monsters available, set the max number of monsters
    if (this->monsterCount > maxMonsters) {
        this->monsterCount = maxMonsters;
    }

    for (index = 0; index < this->monsterCount; index++) {
        placementAttempts = 0;

        speed = u_char(random_number_between(MONSTER_MIN_SPEED, MONSTER_MAX_SPEED));
        classification = 0;

        // If Intelligent
        if (random_number_between(false, true)) {
            classification |= MONSTER_INTELLIGENT_VALUE;
        }
        // If Telepathic
        if (random_number_between(false, true)) {
            classification |= MONSTER_TELEPATHIC_VALUE;
        }
        // If Tunneler
        if (random_number_between(false, true)) {
            classification |= MONSTER_TUNNELER_VALUE;
        }
        // If Erratic
        if (random_number_between(false, true)) {
            classification |= MONSTER_ERRATIC_VALUE;
        }


        // Select random spots until they are only surrounded by room space
        do {
            do {
                monsterRoom = u_char(random_number_between(0, this->roomCount - 1));
            } while (this->rooms.at(monsterRoom)->getId() == playerRoom->getId() && this->rooms.at(monsterRoom)->getFloor() == dungeon->getPlayer()->getFloor());

            // Select random spot inside the room
            monsterX = u_char(random_number_between(this->rooms.at(monsterRoom)->getStartingX(), this->rooms.at(monsterRoom)->getStartingX() + this->rooms.at(monsterRoom)->getWidth() - 1));
            monsterY = u_char(random_number_between(this->rooms.at(monsterRoom)->getStartingY(), this->rooms.at(monsterRoom)->getStartingY() + this->rooms.at(monsterRoom)->getHeight() - 1));

            placementAttempts++;
        } while (this->characters[monsterY][monsterX] != null && placementAttempts < 25);

        // If failed to find, just man handle it through
        if (placementAttempts >= 25) {
            for (roomIndex = 0; roomIndex < this->roomCount; roomIndex++) {
                if (this->rooms.at(roomIndex)->getId() == playerRoom->getId() && this->rooms.at(roomIndex)->getFloor() == dungeon->getPlayer()->getFloor()) {
                    continue;
                }
                // Start looping and find the next open spot
                for (height = this->rooms.at(roomIndex)->getStartingY(); height < this->rooms.at(roomIndex)->getStartingY() + this->rooms.at(roomIndex)->getHeight(); height++) {
                    for (width = this->rooms.at(roomIndex)->getStartingX(); width < this->rooms.at(roomIndex)->getStartingX() + this->rooms.at(roomIndex)->getWidth(); width++) {
                        if (this->characters[height][width] == null) {
                            monsterX = width;
                            monsterY = height;
                        }
                    }
                }
            }
        }

        this->monsters.push_back(new Monster(this, monsterX, monsterY, classification, speed));
        this->characters[monsterY][monsterX] = this->monsters.at(index);
    }

    return this;
}

Floor* Floor::loadBorders() {
    return this;
}

Floor* Floor::loadRock() {
    return this;
}

Floor* Floor::loadRooms() {
    return this;
}

Floor* Floor::loadCorridors() {
    return this;
}

Floor* Floor::loadPlayer() {
    return this;
}

Floor* Floor::loadMonsters() {
    return this;
}