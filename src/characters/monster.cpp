#include "monster.h"

Monster::Monster(Floor* floor, u_char x, u_char y, u_char classification, u_char speed)
        : Character(floor, x, y, convert_base10_to_char(classification), speed, false, true) {
    this->classification = classification;

    this->playerLastSpottedX = 0;
    this->playerLastSpottedY = 0;
}

Monster::~Monster() = default;

int Monster::NextEventTick(Event* event) {
    auto monster = (Monster*) event->character;
    if (monster->getIsAlive()) {
        return monster->getFloor()->getDungeon()->getEventManager()->getTick() + (CHARACTER_SPEED_NUMERATOR / monster->getSpeed());
    } else {
        return -1;
    }
}

int Monster::HandleEvent(Event* event) {
    auto monster = (Monster*) event->character;

    if (monster->getIsAlive()) {
        Floor* floor = monster->getFloor();

        void (* monsterMovement[])(Monster*, u_char*, u_char*) = {
                Monster::Move0, Monster::Move1,
                Monster::Move2, Monster::Move3,
                Monster::Move4, Monster::Move5,
                Monster::Move6, Monster::Move7,
                Monster::Move8, Monster::Move9,
                Monster::Move10, Monster::Move11,
                Monster::Move12, Monster::Move13,
                Monster::Move14, Monster::Move15
        };

        u_char x = monster->getX();
        u_char y = monster->getY();

        monsterMovement[monster->getClassification()](monster, &x, &y);

        // Now a few things could happen
        // 0) The monster just moved on themselves
        // 1) The monster just fell on the player
        // 2) The monster just fell on another monster
        // 3) The monster just fell on open land
        if (x == monster->getX() && y == monster->getY()) { // 0) The monster just moved onto themselves
            return 0;
        } else if (floor->getCharacterAt(x, y) != null) {// 1-2) The monster moved onto someone else
            if (floor->getCharacterAt(x, y)->getIsPlayer()) { // 1) The monster fell on a player
                auto player = (Player*) floor->getCharacterAt(x, y);

                monster->battlePlayer(player);

                if (!monster->getIsAlive()) { // The player survived the battle
                    // Where our glorious monster was now become a corridor if they are a tunneler and were on rock
                    if (monster->isTunneler() && floor->getTerrainAt(monster->getX(), monster->getY())->isRock()) {
                        delete (floor->getTerrainAt(monster->getX(), monster->getY()));
                        floor->setTerrainAt(new Corridor(floor, 0, monster->getX(), monster->getY()), monster->getX(), monster->getY());
                    }
                    return 0;
                }
            } else if (floor->getCharacterAt(x, y)->getIsMonster()) { // The monster fell on another monster
                auto otherMonster = (Monster*) floor->getCharacterAt(x, y);
                Monster* deadMonster;

                monster->battleMonster(otherMonster);

                if (monster->getIsAlive()) {
                    // Our monster lived
                    deadMonster = otherMonster;
                } else {
                    // Other monster lived
                    deadMonster = monster;
                }

                // Where our glorious monster was now become a corridor if they are a tunneler and were on rock
                if (deadMonster->isTunneler() && floor->getTerrainAt(deadMonster->getX(), deadMonster->getY())->isRock()) {
                    delete (floor->getTerrainAt(deadMonster->getX(), deadMonster->getY()));
                    floor->setTerrainAt(new Corridor(floor, 0, deadMonster->getX(), deadMonster->getY()), deadMonster->getX(), deadMonster->getY());
                }

                // Remove this monster's corpse
                floor->setCharacterAt(null, deadMonster->getX(), deadMonster->getY());

                if (!monster->getIsAlive()) { // The other monster won
                    // Stop running this monster
                    return 0;
                }
            } else {
                // Unknown case encountered, exit
                return 0;
            }
        }

        // Regardless, if made down to here, move the monster there
        monster->moveTo(x, y);
    }

    return 0;
}

u_int Monster::AliveCount(Dungeon* dungeon) {
    u_char index;
    u_char monsterIndex;
    u_int monstersAlive = 0;
    for (index = 0; index < dungeon->getFloorCount(); index++) {
        for (monsterIndex = 0; monsterIndex < dungeon->getFloor(index)->getMonsterCount(); monsterIndex++) {
            if (dungeon->getFloor(index)->getMonster(monsterIndex)->getIsAlive()) {
                monstersAlive++;
            }
        }
    }

    return monstersAlive;
}

int Monster::DijkstraCompare(const void* A, const void* B) {
    auto monsterA = (MonsterCost*) A;
    auto monsterB = (MonsterCost*) B;

    return monsterA->getCost() - monsterB->getCost();
}

int Monster::RunDijkstraOnFloor(Floor* floor) {
    return Monster::RunDijkstra(floor, MONSTER_DIJKSTRA_TYPE_TUNNELER, floor->tunnelerView) ||
           Monster::RunDijkstra(floor, MONSTER_DIJKSTRA_TYPE_CHEAPEST_PATH, floor->cheapestPathToPlayer) ||
           Monster::RunDijkstra(floor, MONSTER_DIJKSTRA_TYPE_NON_TUNNELER, floor->nonTunnelerView);
}

int Monster::RunDijkstra(Floor* floor, char type, u_char costChart[DUNGEON_FLOOR_HEIGHT][DUNGEON_FLOOR_WIDTH]) {
    switch (type) {
        case MONSTER_DIJKSTRA_TYPE_TUNNELER:
        case MONSTER_DIJKSTRA_TYPE_NON_TUNNELER:
        case MONSTER_DIJKSTRA_TYPE_CHEAPEST_PATH:
            break;
        default:
            throw "Invalid Dijkstra option presented";
    }

    MonsterCost* monsterCost[DUNGEON_FLOOR_HEIGHT][DUNGEON_FLOOR_WIDTH];
    heap_t heap;
    u_char width;
    u_char height;

    u_char playerX = floor->getDungeon()->getPlayer()->getX();
    u_char playerY = floor->getDungeon()->getPlayer()->getY();

    heap_init(&heap, Monster::DijkstraCompare, null);

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            monsterCost[height][width] = new MonsterCost(floor, width, height, 1);

            // Add in hardness calculation for non zero
            if (type != 0) {
                monsterCost[height][width]->addCost(u_char(floor->getTerrainAt(width, height)->getHardness() / MONSTER_HARDNESS_PER_TURN));
            }

            costChart[height][width] = U_CHAR_MAX;
        }
    }

    monsterCost[playerY][playerX]->setCost(0);
    heap_insert(&heap, monsterCost[playerY][playerX]);

    MonsterCost* item;
    while ((item = (MonsterCost*) heap_remove_min(&heap))) {
        if (!floor->getTerrainAt(item->getX(), item->getY())->isImmutable()) {
            switch (type) {
                case MONSTER_DIJKSTRA_TYPE_TUNNELER: // Type == 1
                    for (height = u_char(item->getY() - 1); height <= item->getY() + 1; height++) {
                        for (width = u_char(item->getX() - 1); width <= item->getX() + 1; width++) {
                            if (height != item->getY() || width != item->getX()) {
                                if (costChart[height][width] >= item->getCost() + monsterCost[height][width]->getCost() && !floor->getTerrainAt(width, height)->isImmutable()) {
                                    monsterCost[height][width]->addCost(item->getCost());
                                    costChart[height][width] = monsterCost[height][width]->getCost();

                                    heap_insert(&heap, monsterCost[height][width]);
                                }
                            }
                        }
                    }
                    break;
                case MONSTER_DIJKSTRA_TYPE_CHEAPEST_PATH: // Type == 0
                    for (height = u_char(item->getY() - 1); height <= item->getY() + 1; height++) {
                        for (width = u_char(item->getX() - 1); width <= item->getX() + 1; width++) {
                            if (height != item->getY() || width != item->getX()) {
                                if (costChart[height][width] >= item->getCost() + monsterCost[height][width]->getCost()) {
                                    monsterCost[height][width]->addCost(item->getCost());
                                    costChart[height][width] = monsterCost[height][width]->getCost();

                                    heap_insert(&heap, monsterCost[height][width]);
                                }
                            }
                        }
                    }
                    break;
                case MONSTER_DIJKSTRA_TYPE_NON_TUNNELER: // Type == -1
                    if (floor->getTerrainAt(item->getX(), item->getY())->isWalkable()) {
                        for (height = u_char(item->getY() - 1); height <= item->getY() + 1; height++) {
                            for (width = u_char(item->getX() - 1); width <= item->getX() + 1; width++) {
                                if (height != item->getY() || width != item->getX()) {
                                    if (costChart[height][width] >= item->getCost() + monsterCost[height][width]->getCost()) {
                                        if (floor->getTerrainAt(width, height)->isWalkable()) {
                                            monsterCost[height][width]->addCost(item->getCost());
                                            costChart[height][width] = monsterCost[height][width]->getCost();

                                            heap_insert(&heap, monsterCost[height][width]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }
    costChart[playerY][playerX] = U_CHAR_MIN;

    for (height = 0; height < DUNGEON_FLOOR_HEIGHT; height++) {
        for (width = 0; width < DUNGEON_FLOOR_WIDTH; width++) {
            delete (monsterCost[height][width]);
        }
    }

    return 0;
}

int Monster::moveTo(u_char toX, u_char toY) {
    Floor* floor = this->getFloor();

    // Tunneler monsters leave corridors behind if tunneling and on rock
    if (this->isTunneler() && floor->getTerrainAt(this->getX(), this->getY())->isRock()) {
        delete (floor->getTerrainAt(this->getX(), this->getY()));
        floor->setTerrainAt(new Corridor(floor, 0, this->getX(), this->getY()), this->getX(), this->getY());
    }

    floor->setCharacterAt(null, this->getX(), this->getY());

    this->setX(toX)->setY(toY);

    floor->setCharacterAt(this, this->getX(), this->getY());

    return 0;
}

void Monster::battleMonster(Monster* otherMonster) {
    // Stronger wins
//    if (this->getLevel() > otherMonster->getLevel()) {
//        otherMonster->killCharacter();
//    } else if (this->getLevel() < otherMonster->getLevel()) {
//        this->killCharacter();
    if (Dice::RandomNumberBetween(false, true)) {
        otherMonster->killCharacter();
    } else {
        this->killCharacter();
    }
}

void Monster::battlePlayer(Player* player) {
    player->battleMonster(this);
}

bool Monster::hasLineOfSightTo(Player* player) {
    Floor* floor = this->getFloor();
    double slope;
    double error = 0.0;

    u_char x;
    u_char y;

    u_char x0 = this->getX();
    u_char x1 = player->getX();

    u_char y0 = this->getY();
    u_char y1 = player->getY();

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

char* Monster::locationString(char location[19]) {
    Player* player = this->getFloor()->getDungeon()->getPlayer();

    char deltaX = player->getX() - this->getX();
    char deltaY = player->getY() - this->getY();

    char north[] = "NORTH";
    char south[] = "SOUTH";
    char east[] = "EAST";
    char west[] = "WEST";

    char deltaYString[10] = "";
    char deltaXString[9] = "";

    u_char index;
    for (index = 0; index < 19; index++) {
        location[index] = '\0';
    }

    if (deltaY != 0) {
        sprintf(deltaYString, "%2d %5s", abs(deltaY), (deltaY > 0 ? south : north));
    }
    if (deltaX != 0) {
        sprintf(deltaXString, "%2d %4s", abs(deltaX), (deltaX > 0 ? east : west));
    }

    sprintf(location, "%8s | %7s", deltaYString, deltaXString);
    location[18] = '\0';

    return location;
}

bool Monster::isIntelligent() {
    return bool(this->classification & MONSTER_INTELLIGENT);
}

bool Monster::isTelepathic() {
    return bool(this->classification & MONSTER_TELEPATHIC);
}

bool Monster::isTunneler() {
    return bool(this->classification & MONSTER_TUNNELER);
}

bool Monster::isErratic() {
    return bool(this->classification & MONSTER_ERRATIC);
}

/** GETTERS **/
std::string Monster::getName() {
    return this->name;
}

std::string Monster::getDescription() {
    return this->description;
}

std::string Monster::getColor() {
    return this->color;
}

u_int Monster::getHitPoints() {
    return this->hitPoints;
}

u_int Monster::getAttackDamage() {
    return this->attackDamage;
}

u_char Monster::getClassification() {
    return this->classification;
}

u_char Monster::getRarity() {
    return this->rarity;
}

u_char Monster::getPlayerLastSpottedX() {
    return this->playerLastSpottedX;
}

u_char Monster::getPlayerLastSpottedY() {
    return this->playerLastSpottedY;
}
/** GETTERS **/

/** SETTERS **/
Monster* Monster::setName(std::string* name) {
    this->name = *name;

    return this;
}

Monster* Monster::setDescription(std::string* description) {
    this->description = *description;

    return this;
}

Monster* Monster::setColor(std::string* color) {
    this->color = *color;

    return this;
}

Monster* Monster::setHitPoints(u_int hitPoints) {
    this->hitPoints = hitPoints;

    return this;
}

Monster* Monster::setAttackDamage(u_int attackDamage) {
    this->attackDamage = attackDamage;

    return this;
}

Monster* Monster::setClassification(u_char classification) {
    this->classification = classification;

    return this;
}

Monster* Monster::setRarity(u_char rarity) {
    this->rarity = rarity;

    return this;
}

Monster* Monster::setPlayerLastSpottedX(u_char playerLastSpottedX) {
    this->playerLastSpottedX = playerLastSpottedX;

    return this;
}

Monster* Monster::setPlayerLastSpottedY(u_char playerLastSpottedY) {
    this->playerLastSpottedY = playerLastSpottedY;

    return this;
}
/** SETTERS **/

/**-------------------- MONSTER COST ----------------------**/
MonsterCost::MonsterCost(Floor* floor, u_char x, u_char y, u_char cost) {
    this->floor = floor;
    this->x = x;
    this->y = y;
    this->cost = cost;
}

MonsterCost::~MonsterCost() = default;

MonsterCost* MonsterCost::addCost(u_char cost) {
    this->cost += cost;

    return this;
}

MonsterCost* MonsterCost::removeCost(u_char cost) {
    if (this->cost - cost < 0) {
        this->cost = 0;
    } else {
        this->cost -= cost;
    }

    return this;
}

/** GETTERS **/
Floor* MonsterCost::getFloor() {
    return this->floor;
}

u_char MonsterCost::getX() {
    return this->x;
}

u_char MonsterCost::getY() {
    return this->y;
}

u_char MonsterCost::getCost() {
    return this->cost;
}
/** GETTERS **/

/** SETTERS **/
MonsterCost* MonsterCost::setFloor(Floor* floor) {
    this->floor = floor;

    return this;
}

MonsterCost* MonsterCost::setX(u_char x) {
    this->x = x;

    return this;
}

MonsterCost* MonsterCost::setY(u_char y) {
    this->y = y;

    return this;
}

MonsterCost* MonsterCost::setCost(u_char cost) {
    this->cost = cost;

    return this;
}
/** SETTERS **/
