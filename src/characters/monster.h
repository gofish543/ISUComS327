#ifndef MONSTER_H
#define MONSTER_H

struct Monster;
typedef struct Monster Monster;

struct MonsterCost;
typedef struct MonsterCost MonsterCost;

#define MONSTER_MIN_SPEED 5
#define MONSTER_MAX_SPEED 20

#define MONSTER_HARDNESS_PER_TURN 85

#define MONSTER_INTELLIGENT_VALUE   0x0001
#define MONSTER_TELEPATHIC_VALUE    0x0002
#define MONSTER_TUNNELER_VALUE      0x0004
#define MONSTER_ERRATIC_VALUE       0x0008

#define MONSTER_INTELLIGENT_LEVEL 8
#define MONSTER_TELEPATHIC_LEVEL 4
#define MONSTER_TUNNELER_LEVEL 2
#define MONSTER_ERRATIC_LEVEL 1

#define monster_is_intelligent(monster)  (monster->classification & 0x0001)
#define monster_is_telepathic(monster)   (monster->classification & 0x0002)
#define monster_is_tunneler(monster)     (monster->classification & 0x0004)
#define monster_is_erratic(monster)      (monster->classification & 0x0008)

#define monster_next_action(monster)   (monster->character->floor->dungeon->eventManager->tick + ( 1000 / monster->speed))

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "character.h"
#include "player.h"
#include "./monster/movement.h"
#include "../vendor/heap.h"
#include "../character_listings.h"
#include "../floor.h"
#include "../dungeon.h"
#include "../resource.h"
#include "../events.h"
#include "../output.h"

struct Monster {
    Character* character;

    u_char classification;
    u_char speed;
    u_char level;
    bool isAlive;

    u_char playerLastSpottedX;
    u_char playerLastSpottedY;
};

struct MonsterCost {
    Floor* floor;

    u_char x;
    u_char y;
    u_char cost;
};

Monster* monster_initialize(Floor* floor, u_char x, u_char y, u_char type, u_char speed);
Monster* monster_terminate(Monster* monster);

int monster_next_tick(Event* event);
int monster_event(Event* event);

int monster_move_to(Monster* monster, u_char toX, u_char toY);

int monsters_battle(Monster* monsterA,Monster* monsterB);
int monster_slain(Monster* monster);
int monster_alive_count(Dungeon* dungeon);

int32_t monster_dijkstra_compare(const void* A, const void* B);
int monster_run_dijkstra_on_floor(Floor* floor);
void monster_run_dijkstra(Floor* floor, char type, u_char costChart[FLOOR_HEIGHT][FLOOR_WIDTH]);

bool monster_has_line_of_sight(Monster* monster);
char* monster_location_string(Monster* monster, char location[19]);

#endif