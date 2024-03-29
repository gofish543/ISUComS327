#include "output.h"

Output::Output(Dungeon* dungeon) {
    this->doNCurses = dungeon->getSettings()->doNCursesPrint();
    this->dontNCurses = !this->doNCurses;
    this->doExpanded = dungeon->getSettings()->doExpandPrint();
    this->window = dungeon->getWindow();
    this->dungeon = dungeon;
}

Output::~Output() = default;

Output* Output::print() {
    // Ncurses clear window
    if (this->doNCurses) {
        clear();
    }

    u_char y;
    u_char x;
    u_char yMin;
    u_char xMin;
    u_char yMax;
    u_char xMax;
    u_char index;
    u_int foreground;
    u_int background;
    Floor* floor = dungeon->getCurrentFloor();

    this->setColor(EFD_COLOR_WHITE, EFD_COLOR_GREY_DARK);

    if (this->dontNCurses) {
        this->print("\n");
    }

    if (this->doExpanded) {
        yMin = 0;
        xMin = 0;
        yMax = DUNGEON_FLOOR_HEIGHT;
        xMax = DUNGEON_FLOOR_WIDTH;

        this->print("   ");
        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            this->print("%3d", x);
        }
        this->print("\n");
    } else {
        yMin = 1;
        xMin = 1;
        yMax = DUNGEON_FLOOR_HEIGHT - 1;
        xMax = DUNGEON_FLOOR_WIDTH - 1;
    }

    for (y = yMin; y < yMax; y++) {
        if (this->doExpanded) {
            this->print("%2d ", y);
        }
        for (x = xMin; x < xMax; x++) {
            floor->getColorAt(x, y, &foreground, &background);
            this->setColor(foreground, background);

            if (this->doExpanded) {
                this->print(" %c ", floor->getOutputCharacterAt(x, y));
            } else {
                this->print("%c", floor->getOutputCharacterAt(x, y));
            }

        }
        this->print("\n");
    }

    this->setColor(EFD_COLOR_WHITE, EFD_COLOR_GREY_DARK);
    for (index = 0; index < DUNGEON_TEXT_LINES; index++) {
        this->print("%s", this->dungeon->getText(index).c_str());
        this->print("\n");
    }

    // Ncurses refresh
    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::print(std::string* format, ...) {
    va_list args;
    va_start(args, format);

    if (this->doNCurses) {
        vw_printw(window, format->c_str(), args);
    } else {
        vprintf(format->c_str(), args);
    }

    va_end(args);

    return this;
}

Output* Output::print(const char* format, ...) {
    va_list args;
    va_start(args, format);

    if (this->doNCurses) {
        vw_printw(window, format, args);
    } else {
        vprintf(format, args);
    }

    va_end(args);

    return this;
}

Output* Output::print(u_int debugFunctions, Floor* floor) {
    if (floor == null) {
        floor = dungeon->getCurrentFloor();
    }

    if (this->doNCurses) {
        clear();
    }

    if (debugFunctions & OUTPUT_DEBUG_HARDNESS) {
        this->printHardness(floor);
    }

    if (debugFunctions & OUTPUT_DEBUG_TUNNELER) {
        this->printTunneler(floor);
    }

    if (debugFunctions & OUTPUT_DEBUG_NON_TUNNELER) {
        this->printNonTunneler(floor);
    }

    if (debugFunctions & OUTPUT_DEBUG_SHORTEST_PATH) {
        this->printShortestPath(floor);
    }

    if (debugFunctions & OUTPUT_DEBUG_MONSTER_TEMPLATES) {
        this->printMonsterTemplates();
    }

    if (debugFunctions & OUTPUT_DEBUG_OBJECT_TEMPLATES) {
        this->printObjectTemplates();
    }

    if (debugFunctions & OUTPUT_DEBUG_TERMINATE) {
        this->printTerminate();
    }

    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::printEndgame() {
    if (this->doNCurses) {
        clear();
    }

    if (this->dungeon->getPlayer()->getRequestTerminate()) {
        this->print("Thank you for playing, safely exiting\n");
    } else if (dungeon->getPlayer()->isAlive() && dungeon->getBoss()->isAlive()) {
        this->print("Queue completely empty, terminating the program safely\n");
    } else {
        u_char y;

        if (this->dontNCurses) {
            for (y = 0; y < DUNGEON_FLOOR_HEIGHT; y++) {
                this->print("\n");
            }
        }

        this->print("+----------------+-------+--- PLAYER  STATISTICS -----------------------------+\n");
        this->print("| Player Level   | %5d |                                                    |\n", this->dungeon->getPlayer()->getLevel());
        this->print("| Days Survived  | %5d |                                                    |\n", this->dungeon->getPlayer()->getDaysSurvived());
        this->print("| Monsters Slain | %5d |                                                    |\n", this->dungeon->getPlayer()->getMonstersSlain());
        this->print("| Alive          | %5d |                                                    |\n", this->dungeon->getPlayer()->isAlive());
        this->print("+----------------+-------+--- PLAYER  STATISTICS -----------------------------+\n");
    }

    if (this->doNCurses) {
        refresh();
        this->print("Press any key to continue...\n");
        getch();
    }

    return this;
}

Output* Output::printMonsterMenu(u_short startIndex, u_short selectedIndex) {
    u_char y;
    u_short index;
    char location[19] = "";

    if (this->doNCurses) {
        clear();
    }

    this->print("+-----+-----+-------+-------+-------+-------+-------+--------------------------+\n");
    this->print("|     | NUM | ALIVE | INTEL | TELEP | TUNNE | ERRAT |         LOCATION         |\n");
    this->print("+-----+-----+-------+-------+-------+-------+-------+--------------------------+\n");
    for (index = startIndex, y = 0; y < DUNGEON_FLOOR_HEIGHT && index < dungeon->getCurrentFloor()->getMonsterCount(); index++, y++) {
        this->print("| [%c] | %3d | %5s | %5s | %5s | %5s | %5s | %24s |\n",
                    selectedIndex == index ? '*' : ' ',
                    index,
                    (dungeon->getCurrentFloor()->getMonster(index)->isAlive()) ? "YES" : "NO",
                    (dungeon->getCurrentFloor()->getMonster(index)->isIntelligent()) ? "YES" : "NO",
                    (dungeon->getCurrentFloor()->getMonster(index)->isTelepathic()) ? "YES" : "NO",
                    (dungeon->getCurrentFloor()->getMonster(index)->isTunneler()) ? "YES" : "NO",
                    (dungeon->getCurrentFloor()->getMonster(index)->isErratic()) ? "YES" : "NO",
                    dungeon->getCurrentFloor()->getMonster(index)->locationString(location)
        );
    }
    this->print("+-----+-----+-------+-------+-------+-------+-------+--------------------------+\n");

    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::printInventory(u_char selectIndex) {
    u_char index;
    Player* player = this->dungeon->getPlayer();
    if (this->doNCurses) {
        clear();
    }

    this->print("+-----+-------+----------------------------------------------+-----------------+\n");
    this->print("|     | INDEX |                     NAME                     |       TYPE      |\n");
    this->print("+-----+-------+----------------------------------------------+-----------------+\n");
    for (index = 0; index < PLAYER_MAX_INVENTORY_SIZE; index++) {
        if (index < player->getInventoryCount()) {
            this->print("| [%c] |     %d | %44s | %15s |\n", selectIndex == index ? '*' : ' ', index, player->getInventoryAt(index)->getName().c_str(), player->getInventoryAt(index)->getTypeString().c_str());
        } else {
            this->print("| [%c] |     %d |                                              |                 |\n", selectIndex == index ? '*' : ' ', index);
        }
    }
    this->print("+-----+-------+----------------------------------------------+-----------------+\n");

    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::printEquipment(u_char selectedIndex) {
    u_char index = 0;
    Player* player = this->dungeon->getPlayer();
    if (this->doNCurses) {
        clear();
    }

    this->print("+-----+--------------+--------+------------------------------------------------+\n");
    this->print("|     |     TYPE     | SYMBOL |                      NAME                      |\n");
    this->print("+-----+--------------+--------+------------------------------------------------+\n");
    for (auto const& equipment : *player->getEquipment()) {
        if (equipment.second == null) {
            this->print("| [%c] | %12s |        |                                                |\n", selectedIndex == index ? '*' : ' ', Object::typeToString(equipment.first).c_str());
        } else {
            this->print("| [%c] | %12s | %6c | %46s |\n", selectedIndex == index ? '*' : ' ', equipment.second->getTypeString().c_str(), equipment.second->getCharacter(), equipment.second->getName().c_str());
        }
        index++;
    }
    this->print("+-----+--------------+--------+------------------------------------------------+\n");

    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::printInspectItem(u_char selectedIndex) {
    u_char index = 0;
    Object* object = this->dungeon->getPlayer()->getInventoryAt(selectedIndex);
    if (this->doNCurses) {
        clear();
    }

    this->print("+----------------+-------------------------------------------------------------+\n");
    this->print("|      NAME      |                            VALUE                            |\n");
    this->print("+----------------+-------------------------------------------------------------+\n");
    this->print("| %14s | %59s |\n", "Name", object->getName().c_str());
    this->print("| %14s | %59s |\n", "Desc", trim(object->getDescription().substr(0, 58), "\t\n\v\f\r").c_str());
    for (index = 1; index <= object->getDescription().size() / 58; index++) {
        this->print("|                | %59s |\n", trim(object->getDescription().substr(index * 58, 58), "\t\n\v\f\r").c_str());
    }
    this->print("| %14s | %59s |\n", "Type", object->getTypeString().c_str());
    this->print("| %14s | %59d |\n", "Color", object->getColor());
    this->print("| %14s | %59s |\n", "Hit", object->getHitDice()->toString().c_str());
    this->print("| %14s | %59s |\n", "Damage", object->getDamageDice()->toString().c_str());
    this->print("| %14s | %59s |\n", "Dodge", object->getDodgeDice()->toString().c_str());
    this->print("| %14s | %59d |\n", "Weight", object->getWeight());
    this->print("| %14s | %59s |\n", "Speed", object->getSpeedDice()->toString().c_str());
    this->print("| %14s | %59s |\n", "Special", object->getSpecialAttributeDice()->toString().c_str());
    this->print("| %14s | %59s |\n", "Value", object->getValueDice()->toString().c_str());
    this->print("| %14s | %59s |\n", "Artifact", object->getIsArtifact() ? "Yes" : "No");
    this->print("| %14s | %59d |\n", "Rarity", object->getRarity());
    this->print("+----------------+-------------------------------------------------------------+\n");

    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::printInspectMonster(u_short selectedIndex) {
    u_char index = 0;
    Monster* monster = this->dungeon->getCurrentFloor()->getMonster(selectedIndex);

    if (this->doNCurses) {
        clear();
    }

    this->print("+----------------+-------------------------------------------------------------+\n");
    this->print("|      NAME      |                            VALUE                            |\n");
    this->print("+----------------+-------------------------------------------------------------+\n");
    this->print("| %14s | %59s |\n", "Name", monster->getName().c_str());
    this->print("| %14s | %59s |\n", "Desc", trim(monster->getDescription().substr(0, 58), "\t\n\v\f\r").c_str());
    for (index = 1; index <= monster->getDescription().size() / 58; index++) {
        this->print("|                | %59s |\n", trim(monster->getDescription().substr(index * 58, 58), "\t\n\v\f\r").c_str());
    }
    this->print("| %14s | %59d |\n", "Color", monster->getColor());
    this->print("| %14s | %59d |\n", "Hit Points", monster->getHitPoints());
    this->print("| %14s | %59s |\n", "Damage", monster->getDamageDice()->toString().c_str());
    this->print("| %14s | %59d |\n", "Abilities", monster->getAbility());
    this->print("| %14s | %59d |\n", "Rarity", monster->getRarity());
    this->print("+----------------+-------------------------------------------------------------+\n");

    if (this->doNCurses) {
        refresh();
    }

    return this;
}

Output* Output::printHardness(Floor* floor) {
    u_char x;
    u_char y;
    Terrain* terrain;

    if (this->doExpanded) {
        this->print("   ");
        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            this->print("%3d", x);
        }
        this->print("\n");
    }

    for (y = 0; y < DUNGEON_FLOOR_HEIGHT; y++) {
        if (this->doExpanded) {
            this->print("%2d ", y);
        }

        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            terrain = floor->getTerrainAt(x, y);

            if (terrain->isBorder()) {
                if (this->doExpanded) {
                    this->print(" %c ", terrain->getCharacter());
                } else {
                    this->print("%c", terrain->getCharacter());
                }
            } else {
                if (this->doExpanded) {
                    this->print("%3d", terrain->getHardness());
                } else {
                    this->print("%d", terrain->getHardness() % 10);
                }
            }
        }

        this->print("\n");
    }

    return this;
}

Output* Output::printTunneler(Floor* floor) {
    u_char x;
    u_char y;
    Terrain* terrain;

    if (this->doExpanded) {
        this->print("   ");
        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            this->print("%3d", x);
        }
        this->print("\n");
    }

    for (y = 0; y < DUNGEON_FLOOR_HEIGHT; y++) {
        if (this->doExpanded) {
            this->print("%2d ", y);
        }

        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            terrain = floor->getTerrainAt(x, y);

            if (terrain->isBorder()) {
                if (this->doExpanded) {
                    this->print(" %c ", terrain->getCharacter());
                } else {
                    this->print("%c", terrain->getCharacter());
                }
            } else {
                if (this->doExpanded) {
                    this->print("%3d", floor->getTunnelerViewAt(x, y));
                } else {
                    this->print("%d", floor->getTunnelerViewAt(x, y) % 10);
                }
            }
        }

        this->print("\n");
    }

    return this;
}

Output* Output::printNonTunneler(Floor* floor) {
    u_char x;
    u_char y;
    Terrain* terrain;

    if (this->doExpanded) {
        this->print("   ");
        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            this->print("%3d", x);
        }
        this->print("\n");
    }

    for (y = 0; y < DUNGEON_FLOOR_HEIGHT; y++) {
        if (this->doExpanded) {
            this->print("%2d ", y);
        }

        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            terrain = floor->getTerrainAt(x, y);

            if (terrain->isBorder()) {
                if (this->doExpanded) {
                    this->print(" %c ", terrain->getCharacter());
                } else {
                    this->print("%c", terrain->getCharacter());
                }
            } else {
                if (this->doExpanded) {
                    this->print("%3d", floor->getNonTunnelerViewAt(x, y));
                } else {
                    this->print("%d", floor->getNonTunnelerViewAt(x, y) % 10);
                }
            }
        }

        this->print("\n");
    }

    return this;
}

Output* Output::printShortestPath(Floor* floor) {
    u_char x;
    u_char y;
    Terrain* terrain;

    if (this->doExpanded) {
        this->print("   ");
        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            this->print("%3d", x);
        }
        this->print("\n");
    }

    for (y = 0; y < DUNGEON_FLOOR_HEIGHT; y++) {
        if (this->doExpanded) {
            this->print("%2d ", y);
        }

        for (x = 0; x < DUNGEON_FLOOR_WIDTH; x++) {
            terrain = floor->getTerrainAt(x, y);

            if (terrain->isBorder()) {
                if (this->doExpanded) {
                    this->print(" %c ", terrain->getCharacter());
                } else {
                    this->print("%c", terrain->getCharacter());
                }
            } else {
                if (this->doExpanded) {
                    this->print("%3d", floor->getCheapestPathToPlayerAt(x, y));
                } else {
                    this->print("%d", floor->getCheapestPathToPlayerAt(x, y) % 10);
                }
            }
        }

        this->print("\n");
    }

    return this;
}

Output* Output::printMonsterTemplates() {
    for (auto const& monsterTemplate: dungeon->getMonsterTemplates()) {
        this->print("Parsed Monster %s\n", monsterTemplate->getName().c_str());
        this->print("\tDescription: %s\n", monsterTemplate->getDescription().c_str());
        this->print("\tColor: %d\n", monsterTemplate->getColor());
        this->print("\tSpeed: %u\n", monsterTemplate->getSpeed());
        this->print("\tAbilities: %d\n", monsterTemplate->getAbilities());
        this->print("\tHit Points: %u\n", monsterTemplate->getHitPoints());
        this->print("\tAttack Damage: %u\n", monsterTemplate->getAttackDamage());
        this->print("\tSymbol: %c\n", monsterTemplate->getSymbol());
        this->print("\tRarity: %d\n", monsterTemplate->getRarity());
        this->print("\n\n");
    }

    return this;
}

Output* Output::printObjectTemplates() {
    for (auto const& objectTemplate: dungeon->getObjectTemplates()) {
        this->print("Parsed Object %s\n", objectTemplate->getName().c_str());
        this->print("\tDescription: %s\n", objectTemplate->getDescription().c_str());
        this->print("\tType: %d\n", objectTemplate->getItemType());
        this->print("\tColor: %d\n", objectTemplate->getColor());
        this->print("\tHit Bonus: %s\n", objectTemplate->getHitBonus()->toString().c_str());
        this->print("\tDamage Bonus: %s\n", objectTemplate->getDamageBonus()->toString().c_str());
        this->print("\tDodge Bonus: %s\n", objectTemplate->getDodgeBonus()->toString().c_str());
        this->print("\tDefense Bonus: %s\n", objectTemplate->getDefenseBonus()->toString().c_str());
        this->print("\tWeight: %s\n", objectTemplate->getWeight()->toString().c_str());
        this->print("\tSpeed Bonus: %s\n", objectTemplate->getSpeedBonus()->toString().c_str());
        this->print("\tSpecial Attribute: %s\n", objectTemplate->getSpecialAttribute()->toString().c_str());
        this->print("\tValue: %s\n", objectTemplate->getValue()->toString().c_str());
        this->print("\tIs Artifact: %d\n", objectTemplate->getIsArtifact());
        this->print("\tRarity: %d\n", objectTemplate->getRarity());

        this->print("\n\n");
    }

    return this;
}

void Output::printTerminate() {
    if (this->doNCurses) {
        endwin();
    }

    printf(SHELL_BG_GREY_DARK "\n");
    printf(SHELL_TEXT_RED "Forceful termination\n");
    printf(SHELL_DEFAULT "\n");
    exit(1);
}

Output* Output::printError(std::string* format, ...) {
    if (this->doNCurses) {
        // If ncurses, clear the screen
        clear();
    }

    // Pull a list of args
    va_list args;
    va_start(args, format);

    if (this->doNCurses) {
        attron(COLOR_PAIR(NCURSES_RED));
        vw_printw(this->window, format->c_str(), args);
        vw_printw(this->window, "Press any key to continue...\n", null);
        refresh();
        attroff(COLOR_PAIR(NCURSES_RED));

        getch();
    } else {
        printf(SHELL_TEXT_RED "");
        vprintf(format->c_str(), args);
        printf(SHELL_DEFAULT "\n");
    }

    va_end(args);

    return this;
}

Output* Output::printError(const char* format, ...) {
    if (this->doNCurses) {
        // If ncurses, clear the screen
        clear();
    }

    // Pull a list of args
    va_list args;
    va_start(args, format);

    if (this->doNCurses) {
        attron(COLOR_PAIR(NCURSES_RED));
        vw_printw(this->window, format, args);
        wprintw(this->window, "Press any key to continue...\n");
        attron(COLOR_PAIR(NCURSES_RED));
        refresh();

        getch();
    } else {
        printf(SHELL_TEXT_RED "");
        vprintf(format, args);
        printf(SHELL_DEFAULT "\n");
    }

    va_end(args);

    return this;
}

void Output::setColor(u_int foreground, u_int background) {
    static const std::string shellColors[] = {
            SHELL_TEXT_BLACK,
            SHELL_TEXT_RED,
            SHELL_TEXT_GREEN,
            SHELL_TEXT_YELLOW,
            SHELL_TEXT_BLUE,
            SHELL_TEXT_PURPLE,
            SHELL_TEXT_CYAN,
            SHELL_TEXT_WHITE,
            SHELL_BG_GREY_LIGHT,
            SHELL_BG_GREY_DARK,
    };

    static const int nCursesColors[] = {
            NCURSES_BLACK,
            NCURSES_RED,
            NCURSES_GREEN,
            NCURSES_YELLOW,
            NCURSES_BLUE,
            NCURSES_MAGENTA,
            NCURSES_CYAN,
            NCURSES_WHITE
    };

    if (this->doNCurses) {
        if (background == EFD_COLOR_GREY_DARK) {
            attron(COLOR_PAIR(NCURSES_BG_GREY_DARK | nCursesColors[foreground]));
        } else {
            attron(COLOR_PAIR(NCURSES_BG_GREY_LIGHT | nCursesColors[foreground]));
        }
    } else {
        printf("%s%s", shellColors[background].c_str(), shellColors[foreground].c_str());

    }
}

void Output::resetColor(u_int foreground, u_int background) {
    static const int nCursesColors[] = {
            NCURSES_BLACK,
            NCURSES_RED,
            NCURSES_GREEN,
            NCURSES_YELLOW,
            NCURSES_BLUE,
            NCURSES_MAGENTA,
            NCURSES_CYAN,
            NCURSES_WHITE
    };

    if (this->doNCurses) {
        if (background == EFD_COLOR_GREY_DARK) {
            attron(COLOR_PAIR(NCURSES_BG_GREY_DARK | nCursesColors[foreground]));
        } else {
            attron(COLOR_PAIR(NCURSES_BG_GREY_LIGHT | nCursesColors[foreground]));
        }
    } else {
        printf(SHELL_DEFAULT);
    }
}