#ifndef BORDER_H
#define BORDER_H

#include "../../include/forward_declarations.h"
#include "../character_listings.h"
#include "../floor.h"
#include "terrain.h"

namespace App {
    class Border : public Terrain {
    public:
        Border(Floor* floor, u_short id, u_char x, u_char y, u_char character);

    protected:

    private:
    };
}

using App::Floor;
using App::Terrain;
using App::Border;

#endif