#include "player.h"

Player::Player(Floor* floor, u_char x, u_char y) : Character(floor, x, y, PLAYER_CHARACTER, PLAYER_SPEED, true, false) {
    u_char height;
    u_char width;

    this->takingStaircase = null;

    this->requestTerminate = false;

    this->level = 0;
    this->monstersSlain = 0;
    this->daysSurvived = 0;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            this->visibility[height][width] = null;
        }
    }
}

Player::Player(Floor* floor, u_char x, u_char y, u_int level, u_int monstersSlain, u_int daysSurvived)
        : Player(floor, x, y) {
    this->takingStaircase = null;

    this->requestTerminate = false;

    this->level = level;
    this->monstersSlain = monstersSlain;
    this->daysSurvived = daysSurvived;
}

Player::~Player() = default;

int Player::NextEventTick(Event* event) {
    auto player = (Player*) event->character;

    if (player->getIsAlive()) {
        return player->getFloor()->getDungeon()->getEventManager()->getTick() + (CHARACTER_SPEED_NUMERATOR / player->getSpeed());
    } else {
        return -1;
    }
}

int Player::HandleEvent(Event* event) {
    int move;
    auto player = (Player*) event->character;
    Dungeon* dungeon = player->getFloor()->getDungeon();

    move = getChar(dungeon->getWindow(), dungeon->getSettings()->doNCursesPrint());

    switch (move) {
        case 'm':
            player->handleEventKeyMonsterMenu();

            return Player::HandleEvent(event);
        case 'f':
            player->handleEventKeyToggleFog();

            return Player::HandleEvent(event);
        case 't':
            player->handleEventKeyTeleport();

            return 0;
        case '<':
        case '>':
            if (player->handleEventKeyStaircase(move)) {
                return Player::HandleEvent(event);
            }
            return 0;
        default:
            if (player->handleEventKeyMovement(move)) {
                return Player::HandleEvent(event);
            }
            return 0;
    }
}

int Player::handleEventKeyMonsterMenu() {
    Dungeon* dungeon = this->getFloor()->getDungeon();
    u_short startIndex = 0;
    int character = 0;

    while (character != 27) {
        output_print_current_floor_monster_menu(dungeon, startIndex);

        print(dungeon->getWindow(), dungeon->getSettings()->doNCursesPrint(), "Esc: Close\nArrowUp: Scroll Down\nArrowDown: Scroll Up\n");
        character = getChar(dungeon->getWindow(), dungeon->getSettings()->doNCursesPrint());

        switch (character) {
            case KEY_DOWN:
                if (startIndex < dungeon->getCurrentFloor()->getMonsterCount()) {
                    startIndex++;
                }
                break;
            case KEY_UP:
                if (startIndex > 0) {
                    startIndex--;
                }
                break;
        }
    }

    output(dungeon, output_print_current_floor);

    return 0;
}

int Player::handleEventKeyToggleFog() {
    this->getFloor()->getDungeon()->getSettings()->setFogOfWar(
            !this->getFloor()->getDungeon()->getSettings()->doFogOfWar()
    );

    output(this->getFloor()->getDungeon(), output_print_current_floor);

    return 0;
}

int Player::handleEventKeyTeleport() {
    Dungeon* dungeon = this->getFloor()->getDungeon();

    // Store away original settings
    bool originalFogOfWar = dungeon->getSettings()->doFogOfWar();
    u_char originalX = this->getX();
    u_char originalY = this->getY();
    int character = 0;
    Character* save = null;

    // Change user character, print, and pull from map
    this->setCharacter(PLAYER_TELEPORT_CHARACTER);
    dungeon->getSettings()->setFogOfWar(false);

    output(this->getFloor()->getDungeon(), output_print_current_floor);

    this->getFloor()->setCharacterAt(null, this->x, this->y);

    while (character != 27 && character != 't' && character != 'r') {
        character = getChar(dungeon->getWindow(), dungeon->getSettings()->doNCursesPrint());
        this->getFloor()->setCharacterAt(null, this->getX(), this->getY());

        switch (character) {
            case KEY_UP:
                this->y = u_char(std::max(1, this->y - 1));
                break;
            case KEY_DOWN:
                this->y = u_char(std::min(DUNGEON_FLOOR_HEIGHT - 1, this->y + 1));
                break;
            case KEY_RIGHT:
                this->x = u_char(std::min(DUNGEON_FLOOR_WIDTH - 1, this->x + 1));
                break;
            case KEY_LEFT:
                this->x = u_char(std::max(1, this->x - 1));
                break;
        }

        // Restore previous spot
        if (save != null) {
            this->getFloor()->setCharacterAt(save, save->getX(), save->getY());
        }

        // Store away target spot
        save = this->getFloor()->getCharacterAt(this->x, this->y);

        // Move our "teleport" character there
        this->getFloor()->setCharacterAt(this, this->x, this->y);

        // Print
        output(this->getFloor()->getDungeon(), output_print_current_floor);
    }

    switch (character) {
        case 27: // Esc
            // Revert to original characters
            this->getFloor()->setCharacterAt(null, this->x, this->y);
            this->setX(originalX)->setY(originalY);
            break;
        case 't':
            break;
        case 'r':
            this->getFloor()->setCharacterAt(null, this->x, this->y);

            Room* randomRoom = this->getFloor()->getRoom(u_short(Dice::RandomNumberBetween(0, this->getFloor()->getRoomCount() - 1)));
            this->setX(u_char(Dice::RandomNumberBetween(randomRoom->getStartingX(), randomRoom->getStartingX() + randomRoom->getWidth() - 1)));
            this->setY(u_char(Dice::RandomNumberBetween(randomRoom->getStartingY(), randomRoom->getStartingY() + randomRoom->getHeight() - 1)));

            // Restore previous location
            if (save != null) {
                this->getFloor()->setCharacterAt(save, save->getX(), save->getY());
            }

            save = this->getFloor()->getCharacterAt(this->x, this->y);
            break;
    }

    if (save != null && save->getIsMonster()) {
        // Fight monster
        this->battleMonster((Monster*) save);

        // If didn't survive, place back character
        if (!this->getIsAlive()) {
            this->getFloor()->setCharacterAt(save, save->getX(), save->getY());
        }
    }

    // Restore character
    this->setCharacter(PLAYER_CHARACTER);

    // Place back and restore original fog of war
    this->getFloor()->setCharacterAt(this, this->getX(), this->getY());
    dungeon->getSettings()->setFogOfWar(originalFogOfWar);

    // Update visibility
    this->updateVisibility();

    output(this->getFloor()->getDungeon(), output_print_current_floor);

    return 0;
}

int Player::handleEventKeyStaircase(int command) {
    Dungeon* dungeon = this->getFloor()->getDungeon();
    Floor* floor = this->getFloor();
    Terrain* onTerrain = floor->getTerrainAt(this->x, this->y);
    // We are standing on a staircase
    if (onTerrain->getCharacter() == STAIRCASE_DOWN_CHARACTER || onTerrain->getCharacter() == STAIRCASE_UP_CHARACTER) {
        if ((command == STAIRCASE_DOWN_CHARACTER && onTerrain->getCharacter() == STAIRCASE_DOWN_CHARACTER) ||
            (command == STAIRCASE_UP_CHARACTER && onTerrain->getCharacter() == STAIRCASE_UP_CHARACTER)) {
            // Trying to take a down staircase, and standing on a down staircase
            // Trying to take an up staircase and standing on an up staircase
            this->takingStaircase = (Staircase*) onTerrain;
            dungeon->prependText("Moving %s a staircase", (this->takingStaircase->isUp() ? "Up" : "Down"));
            return 0;
        }
    }

    dungeon->prependText("Not standing on a valid staircase");
    output(dungeon, output_print_current_floor);

    // Not on staircase, cant take a staircase
    // Or not on a staircase facing the right direction
    return 1;
}

int Player::handleEventKeyMovement(int command) {
    Dungeon* dungeon = this->getFloor()->getDungeon();

    u_char x = dungeon->getPlayer()->getX();
    u_char y = dungeon->getPlayer()->getY();

    switch (command) {
        // Upper Left
        case '7':
        case 'y':
            dungeon->prependText("Moving upper left");
            y--;
            x--;
            break;
            // Up
        case '8':
        case 'k':
            dungeon->prependText("Moving up");
            y--;
            break;
            // Upper right
        case '9':
        case 'u':
            dungeon->prependText("Moving upper right");
            y--;
            x++;
            break;
            // Left
        case '4':
        case 'h':
            dungeon->prependText("Moving left");
            x--;
            break;
            // Right
        case '6':
        case 'l':
            dungeon->prependText("Moving right");
            x++;
            break;
            // Down left
        case '1':
        case 'b':
            dungeon->prependText("Moving down left");
            y++;
            x--;
            break;
            // Down
        case '2':
        case 'j':
            dungeon->prependText("Moving down");
            y++;
            break;
            // Down right
        case '3':
        case 'n':
            dungeon->prependText("Moving down right");
            y++;
            x++;
            break;
        case 'Q':
            dungeon->getPlayer()->setRequestTerminate(true);
            // Rest for a turn
        case '5':
        case '.':
            dungeon->prependText("A day of rest...");
            break;
        default:
            dungeon->prependText("Invalid key entered");
            output(dungeon, output_print_current_floor);
            return 1;
    }

    if (!dungeon->getCurrentFloor()->getTerrainAt(x, y)->isWalkable()) {
        dungeon->prependText("Terrain is not walkable, invalid key entered");
        output(dungeon, output_print_current_floor);
        return 1;
    }

    this->moveTo(x, y);

    return 0;
}

int Player::moveTo(u_char toX, u_char toY) {
    Floor* floor = this->getFloor();

    // If moving to the same spot, just exit
    if (toX == this->getX() && toY == this->getY()) {
        return 0;
    }

    // Check out target location
    if (floor->getCharacterAt(toX, toY) != null) {
        this->battleMonster((Monster*) floor->getCharacterAt(toX, toY));

        if (!this->isAlive) {
            // Player died, don't move to spot
            return 0;
        }
    }

    // Move the character to the requested spot
    floor->setCharacterAt(null, this->x, this->y);

    this->x = toX;
    this->y = toY;

    floor->setCharacterAt(this, this->x, this->y);

    Monster::RunDijkstraOnFloor(floor);

    this->updateVisibility();

    return 0;
}

void Player::battleMonster(Monster* monster) {
    if (Monster::AliveCount(this->floor->getDungeon()) == 1) {
        // Player died
        this->killCharacter();

        // Remove player corps
        this->floor->setCharacterAt(null, this->getX(), this->getY());

    } else {
        // Monster died
        monster->killCharacter();

        // Remove monster corps
        this->floor->setCharacterAt(null, monster->getX(), monster->getY());

        // Add level to the player
//        this->addLevel(monster->getLevel());
        this->incrementMonstersSlain();
    }
}

Player* Player::addLevel(int amount) {
    this->level += amount;

    return this;
}

Player* Player::removeLevel(int amount) {
    if (this->level - amount < 0) {
        this->level = 0;
    } else {
        this->level -= amount;
    }

    return this;
}

Player* Player::updateVisibility() {
    u_char height;
    u_char width;
    u_char minHeight = u_char(std::max(1, this->getY() - PLAYER_DEFAULT_LIGHT_RADIUS));
    u_char maxHeight = u_char(std::min(DUNGEON_FLOOR_HEIGHT - 1, this->getY() + PLAYER_DEFAULT_LIGHT_RADIUS));
    u_char minWidth = u_char(std::max(1, this->getX() - PLAYER_DEFAULT_LIGHT_RADIUS));
    u_char maxWidth = u_char(std::min(DUNGEON_FLOOR_WIDTH - 1, this->getX() + PLAYER_DEFAULT_LIGHT_RADIUS));

    for (height = minHeight; height < maxHeight; height++) {
        for (width = minWidth; width < maxWidth; width++) {
            if (this->hasLineOfSightTo(width, height)) {
                this->visibility[height][width] = this->getFloor()->getTerrainAt(width, height);
            }
        }
    }

    return this;
}

bool Player::hasLineOfSightTo(u_char width, u_char height) {
    Floor* floor = this->getFloor();
    double slope;
    double error = 0.0;

    u_char x;
    u_char y;

    u_char x0 = this->getX();
    u_char x1 = width;

    u_char y0 = this->getY();
    u_char y1 = height;

    char deltaX = x1 - x0;
    char deltaY = y1 - y0;

    if (deltaX == 0) {
        // Horizontal line case
        x = x0;
        for (y = y0; y != y1; y += get_sign(deltaY)) {
            if (floor->getTerrainAt(u_char(x), u_char(y))->isRock() || floor->getTerrainAt(u_char(x), u_char(y))->isImmutable()) {
                return false;
            }
        }
    } else {
        slope = abs(int(double(deltaY) / double(deltaX)));
        y = y0;
        for (x = x0; abs(x1 - x) != 0; x += get_sign(deltaX)) {
            if (floor->getTerrainAt(u_char(x), u_char(y))->isRock() || floor->getTerrainAt(u_char(x), u_char(y))->isImmutable()) {
                return false;
            }

            error += slope;
            if (error >= 0.5) {
                y += get_sign(deltaY);
                error -= 1.0;
            }
        }
        // Finish out vertical line case
        for (; abs(y1 - y) != 0; y += get_sign(deltaY)) {
            if (floor->getTerrainAt(u_char(x), u_char(y))->isRock() || floor->getTerrainAt(u_char(x), u_char(y))->isImmutable()) {
                return false;
            }
        }
    }
    return true;
}

Player* Player::incrementMonstersSlain() {
    this->monstersSlain++;

    return this;
}

Player* Player::incrementDaysSurvived() {
    this->daysSurvived++;

    return this;
}

/** GETTERS **/
Staircase* Player::getTakingStaircase() {
    return this->takingStaircase;
}

bool Player::getRequestTerminate() {
    return this->requestTerminate;
}

u_int Player::getLevel() {
    return this->level;
}

u_int Player::getMonstersSlain() {
    return this->monstersSlain;
}

u_int Player::getDaysSurvived() {
    return this->daysSurvived;
}
/** GETTERS **/

/** SETTERS **/
Player* Player::setTakingStaircase(Staircase* takingStaircase) {
    this->takingStaircase = takingStaircase;

    return this;
}

Player* Player::setRequestTerminate(bool requestTerminate) {
    this->requestTerminate = requestTerminate;

    return this;
}

Player* Player::setLevel(u_int level) {
    this->level = level;

    return this;
}

Player* Player::setMonstersSlain(u_int monstersSlain) {
    this->monstersSlain = monstersSlain;

    return this;
}

Player* Player::setDaysSurvived(u_int daysSurvived) {
    this->daysSurvived = daysSurvived;

    return this;
}
/** SETTERS **/
