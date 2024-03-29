#ifndef OBJECT_TEMPLATE_H
#define OBJECT_TEMPLATE_H

#define OBJECT_TEMPLATE_HEADING "RLG327 OBJECT DESCRIPTION 1"

#define OBJECT_TEMPLATE_NAME                "NAME"
#define OBJECT_TEMPLATE_DESCRIPTION         "DESC"
#define OBJECT_TEMPLATE_TYPE                "TYPE"
#define OBJECT_TEMPLATE_COLOR               "COLOR"
#define OBJECT_TEMPLATE_HIT_BONUS           "HIT"
#define OBJECT_TEMPLATE_DAMAGE_BONUS        "DAM"
#define OBJECT_TEMPLATE_DODGE_BONUS         "DODGE"
#define OBJECT_TEMPLATE_DEFENSE_BONUS       "DEF"
#define OBJECT_TEMPLATE_WEIGHT              "WEIGHT"
#define OBJECT_TEMPLATE_SPEED_BONUS         "SPEED"
#define OBJECT_TEMPLATE_SPECIAL_ATTRIBUTE   "ATTR"
#define OBJECT_TEMPLATE_VALUE               "VAL"
#define OBJECT_TEMPLATE_ARTIFACT_STATUS     "ATR"
#define OBJECT_TEMPLATE_RARITY              "RRTY"

#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#include <forward_declarations.h>
#include <colors.h>
#include <global.h>

#include "../resource.h"

namespace App {
    class ObjectTemplate {
    public:
        explicit ObjectTemplate(std::string& templateString);
        ~ObjectTemplate();

        static std::vector<ObjectTemplate*> GenerateTemplates(std::ifstream* inputFile);

        Object* generateObject(Floor* floor, u_char x, u_char y);

        /** GETTERS **/
        std::string getName();
        std::string getDescription();
        u_int getItemType();
        u_short getColor();
        Dice* getHitBonus();
        Dice* getDamageBonus();
        Dice* getDodgeBonus();
        Dice* getDefenseBonus();
        Dice* getWeight();
        Dice* getSpeedBonus();
        Dice* getSpecialAttribute();
        Dice* getValue();
        bool getIsArtifact();
        u_char getRarity();
        /** GETTERS **/

        /** SETTERS **/
        /** SETTERS **/
    protected:

    private:
        std::string name;
        std::string description;
        u_int itemType;
        u_short color;
        Dice* hitBonus;
        Dice* damageBonus;
        Dice* dodgeBonus;
        Dice* defenseBonus;
        Dice* weight;
        Dice* speedBonus;
        Dice* specialAttribute;
        Dice* value;
        bool isArtifact;
        u_char rarity;
    };
}

#include "../objects/object.h"
#include "../dice.h"

#endif
