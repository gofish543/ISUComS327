#include "monster.h"

Monster* monster_initialize(Floor* floor, u_char x, u_char y, u_char type, u_char speed) {
    Monster* monster = malloc(sizeof(Monster));

    monster->classification = type;
    monster->speed = speed;
    monster->isAlive = true;
    monster->playerLastSpottedX = 0;
    monster->playerLastSpottedY = 0;

    monster->level =
            (monster_is_intelligent(monster) ? MONSTER_INTELLIGENT_LEVEL : 0) +
            (monster_is_telepathic(monster) ? MONSTER_TELEPATHIC_LEVEL : 0) +
            (monster_is_tunneler(monster) ? MONSTER_TUNNELER_LEVEL : 0) +
            (monster_is_erratic(monster) ? MONSTER_ERRATIC_LEVEL : 0) +
            1;

    monster->character = character_initialize(floor, monster, null, x, y);

    return monster;
}

Monster* monster_terminate(Monster* monster) {
    monster->character = character_terminate(monster->character);
    free(monster);

    return null;
}

int monster_next_tick(Event* event) {
    Monster* monster = (Monster*) event->structure;

    if (monster->isAlive) {
        return monster_next_action(monster);
    } else {
        return -1;
    }
}

int monster_event(Event* event) {
    Monster* monster = (Monster*) event->structure;

    // Only move alive monsters
    if (monster->isAlive) {
        Floor* floor = monster->character->floor;

        void (* monster_movement[])(Monster*, u_char*, u_char*) = {
                monster_move_0, monster_move_1,
                monster_move_2, monster_move_3,
                monster_move_4, monster_move_5,
                monster_move_6, monster_move_7,
                monster_move_8, monster_move_9,
                monster_move_10, monster_move_11,
                monster_move_12, monster_move_13,
                monster_move_14, monster_move_15
        };
        u_char x = monster->character->x;
        u_char y = monster->character->y;

        monster_movement[monster->classification](monster, &x, &y);

        // Now a few things could happen
        // 0) The monster just moved on themselves
        // 1) The monster just fell on the player
        // 2) The monster just fell on another monster
        // 3) The monster just fell on open land
        if (x == monster->character->x && y == monster->character->y) { // 0) The monster just moved onto themselves
            return 0;
        } else if (floor->characters[y][x] != null) {
            if (floor->characters[y][x]->player != null) { // 1) The monster fell on the player
                // Let the battle happen
                if (action_player_vs_monster(floor->characters[y][x]->player, monster)) {
                    // The player won if it is true
                    if (monster_is_tunneler(monster) && floor->terrains[monster->character->y][monster->character->x]->isRock) {
                        floor->terrains[monster->character->y][monster->character->x]->isRock = false;
                        floor->terrains[monster->character->y][monster->character->x]->isWalkable = true;
                        floor->terrains[monster->character->y][monster->character->x]->hardness = CORRIDOR_HARDNESS;
                        floor->terrains[monster->character->y][monster->character->x]->character = CORRIDOR_CHARACTER;
                    }
                    return 0;
                }
            } else if (floor->characters[y][x]->monster != null) {// 2) The monster fell on another monster
                // Let the battle happen
                if (action_monster_vs_monster(monster, floor->characters[y][x]->monster) == -1) {
                    // The other monster won
                    return 0;
                }
            }
        }
        monster_move_to(monster, x, y);
    }
    return 0;
}

int monster_move_to(Monster* monster, u_char toX, u_char toY) {
    Floor* floor = monster->character->floor;

    // Tunneler monsters leave corridors behind if tunneling
    if (monster_is_tunneler(monster) && floor->terrains[monster->character->y][monster->character->x]->isRock) {
        floor->terrains[monster->character->y][monster->character->x]->isRock = false;
        floor->terrains[monster->character->y][monster->character->x]->isWalkable = true;
        floor->terrains[monster->character->y][monster->character->x]->hardness = CORRIDOR_HARDNESS;
        floor->terrains[monster->character->y][monster->character->x]->character = CORRIDOR_CHARACTER;
    }
    // Remove where they were previously standing
    floor->characters[monster->character->y][monster->character->x] = null;

    // Update the character's x and y
    monster->character->x = toX;
    monster->character->y = toY;

    // Simply move the player there
    floor->characters[monster->character->y][monster->character->x] = monster->character;

    return 0;
}

int monsters_battle(Monster* monsterA, Monster* monsterB) {
    if (monsterA->level > monsterB->level) {
        return 1;
    } else if (monsterA->level < monsterB->level) {
        return -1;
    } else if (random_number_between(false, true)) {
        return 1;
    } else {
        return -1;
    }
}

int monster_slain(Monster* monster) {
    // Get all monsters on the current floor
    monster->isAlive = false;

    return monster->level;
}

int monster_alive_count(Dungeon* dungeon) {
    u_char index;
    u_short monsterIndex;
    u_int total;
    for (index = 0, total = 0; index < dungeon->floorCount; index++) {
        for (monsterIndex = 0; monsterIndex < dungeon->floors[index]->monsterCount; monsterIndex++) {
            if (dungeon->floors[index]->monsters[monsterIndex]->isAlive) {
                total++;
            }
        }
    }

    return total;
}

int32_t monster_dijkstra_compare(const void* A, const void* B) {
    MonsterCost* monsterA = (MonsterCost*) A;
    MonsterCost* monsterB = (MonsterCost*) B;

    return monsterA->cost - monsterB->cost;
}

int monster_run_dijkstra_on_floor(Floor* floor) {
    monster_run_dijkstra(floor, 1, floor->tunnelerView);
    monster_run_dijkstra(floor, -1, floor->nonTunnelerView);
    monster_run_dijkstra(floor, 0, floor->cheapestPathToPlayer);
    return 0;
}

void monster_run_dijkstra(Floor* floor, char type, u_char costChart[FLOOR_HEIGHT][FLOOR_WIDTH]) {
    MonsterCost* monsterCost[FLOOR_HEIGHT][FLOOR_WIDTH];
    heap_t heap;
    u_char width;
    u_char height;

    u_char playerX = floor->dungeon->player->character->x;
    u_char playerY = floor->dungeon->player->character->y;

    heap_init(&heap, monster_dijkstra_compare, null);

    for (height = 0; height < FLOOR_HEIGHT; height++) {
        for (width = 0; width < FLOOR_WIDTH; width++) {
            monsterCost[height][width] = malloc(sizeof(MonsterCost));

            monsterCost[height][width]->y = height;
            monsterCost[height][width]->x = width;
            // Add in hardness calculation for non zero
            if (type != 0) {
                monsterCost[height][width]->cost = 1 + (floor->terrains[height][width]->hardness / MONSTER_HARDNESS_PER_TURN);
            } else {
                monsterCost[height][width]->cost = 1;
            }

            monsterCost[height][width]->floor = floor;

            costChart[height][width] = U_CHAR_MAX;
        }
    }

    monsterCost[playerY][playerX]->cost = 0;
    heap_insert(&heap, monsterCost[playerY][playerX]);

    MonsterCost* item;
    while ((item = heap_remove_min(&heap))) {
        if (!floor->terrains[item->y][item->x]->isImmutable) {
            if (type == 1) {
                for (height = item->y - 1; height <= item->y + 1; height++) {
                    for (width = item->x - 1; width <= item->x + 1; width++) {
                        if (height != item->y || width != item->x) {
                            if (costChart[height][width] >= item->cost + monsterCost[height][width]->cost && !floor->terrains[height][width]->isImmutable) {
                                monsterCost[height][width]->cost += item->cost;
                                costChart[height][width] = monsterCost[height][width]->cost;

                                heap_insert(&heap, monsterCost[height][width]);
                            }
                        }
                    }
                }
            } else if (type == -1 && floor->terrains[item->y][item->x]->isWalkable) {
                for (height = item->y - 1; height <= item->y + 1; height++) {
                    for (width = item->x - 1; width <= item->x + 1; width++) {
                        if (height != item->y || width != item->x) {
                            if (costChart[height][width] >= item->cost + monsterCost[height][width]->cost) {
                                if (floor->terrains[height][width]->isWalkable) {
                                    monsterCost[height][width]->cost += item->cost;
                                    costChart[height][width] = monsterCost[height][width]->cost;

                                    heap_insert(&heap, monsterCost[height][width]);
                                }
                            }
                        }
                    }
                }
            } else {
                for (height = item->y - 1; height <= item->y + 1; height++) {
                    for (width = item->x - 1; width <= item->x + 1; width++) {
                        if (height != item->y || width != item->x) {
                            if (costChart[height][width] >= item->cost + monsterCost[height][width]->cost) {
                                monsterCost[height][width]->cost += item->cost;
                                costChart[height][width] = monsterCost[height][width]->cost;

                                heap_insert(&heap, monsterCost[height][width]);
                            }
                        }
                    }
                }
            }
        }
    }
    costChart[playerY][playerX] = U_CHAR_MIN;

    for (height = 0; height < FLOOR_HEIGHT; height++) {
        for (width = 0; width < FLOOR_WIDTH; width++) {
            free(monsterCost[height][width]);
        }
    }
}

bool monster_has_line_of_sight(Monster* monster) {
    Floor* floor = monster->character->floor;

    double slope;
    double error = 0.0;

    u_char x;
    u_char y;

    u_char x0 = monster->character->x;
    u_char x1 = monster->character->floor->dungeon->player->character->x;

    u_char y0 = monster->character->y;
    u_char y1 = monster->character->floor->dungeon->player->character->y;

    char deltaX = x1 - x0;
    char deltaY = y1 - y0;

    if (deltaX == 0) {
        // We are standing on the player... what
        slope = 0;
        x = x0;
        for (y = y0; y != y1; y += get_sign(deltaY)) {
            if (floor->terrains[(int) y][(int) x]->isRock || floor->terrains[(int) y][(int) x]->isImmutable) {
                return false;
            }
        }
    } else {
        slope = abs(((double) deltaY) / ((double) deltaX));
        y = y0;
        for (x = x0; abs(x1 - x) != 0; x += get_sign(deltaX)) {
            if (floor->terrains[(int) y][(int) x]->isRock || floor->terrains[(int) y][(int) x]->isImmutable) {
                return false;
            }

            error += slope;
            if (error >= 0.5) {
                y += get_sign(deltaY);
                error -= 1.0;
            }
        }
        for (y = y; abs(y1 - y) != 0; y += get_sign(deltaY)) {
            if (floor->terrains[(int) y][(int) x]->isRock || floor->terrains[(int) y][(int) x]->isImmutable) {
                return false;
            }
        }
    }
    return true;
}

char* monster_location_string(Monster* monster, char location[19]) {
    Player* player = monster->character->floor->dungeon->player;

    char deltaX = player->character->x - monster->character->x;
    char deltaY = player->character->y - monster->character->y;

    char north[] = "NORTH";
    char south[] = "SOUTH";
    char east[] = "EAST";
    char west[] = "WEST";

    char deltaYString[9] = "";
    char deltaXString[8] = "";

    u_char index;
    for (index = 0; index < 19; index++) {
        location[0] = '\0';
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