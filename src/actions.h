#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "dungeon.h"
#include "resource.h"
#include "output.h"
#include "characters/monster.h"
#include "characters/player.h"

void action_player_vs_monster(Player* player, Monster* monster);
void action_monster_vs_monster(Monster* monsterA, Monster* monsterB);

#endif
