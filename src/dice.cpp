#include <climits>
#include "dice.h"

Dice::Dice(int base, u_short rolls, u_short sides) {
    this->base = base;
    this->rolls = rolls;
    this->sides = sides;

    this->result = 0;
}

Dice::Dice(std::string* diceString) {
    // String format is <base> + <rolls> d <sides>
    std::string firstDelimiter = "+";
    std::string secondDelimiter = "d";

    char indexOfPlus = diceString->find(firstDelimiter);
    char indexOfD = diceString->find(secondDelimiter);

    if (indexOfPlus == std::string::npos || indexOfD == std::string::npos) {
        throw "Unknown Dice string parsed";
    }

    try {
        this->result = 0;

        if (abs(std::stoi(diceString->substr(0, indexOfPlus))) > UINT_MAX / 2) {
            this->result = -1;
        }
        if (std::stoi(diceString->substr(indexOfPlus + 1, indexOfD)) > U_SHORT_MAX) {
            this->result = -1;
        }
        if (std::stoi(diceString->substr(indexOfD + 1)) > U_SHORT_MAX) {
            this->result = -1;
        }

        this->base = std::stoi(diceString->substr(0, indexOfPlus));
        this->rolls = std::stoi(diceString->substr(indexOfPlus + 1, indexOfD));
        this->sides = std::stoi(diceString->substr(indexOfD + 1));
    } catch (std::exception& exception) {
        this->result = -1;
    }
}

Dice::~Dice() = default;

int Dice::RandomNumberBetween(int min, int max) {
    static int dice_random_offset = 0;

    int fd = open("/dev/urandom", O_RDWR);
    struct timeval time = {};
    ::gettimeofday(&time, null);
    u_int defaultSeed;
    if (fd == 0) {
        defaultSeed = hash3((u_int) time.tv_sec, (u_int) time.tv_usec, (u_int) getpid());
    } else {
        size_t buffer[1];
        lseek(fd, dice_random_offset, 0);
        if (read(fd, buffer, sizeof(size_t)) != sizeof(size_t)) {
            defaultSeed = hash3((u_int) time.tv_sec, (u_int) time.tv_usec, (u_int) getpid());
        } else {
            defaultSeed = (u_int) buffer[0];
            dice_random_offset += 8;

            if (dice_random_offset > 2048) {
                dice_random_offset = 0;
            }
        }
        close(fd);
    }

    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    randomEngine.seed(defaultSeed);

    return ((u_int) randomEngine()) % ((max + 1) - min) + min;
}

int Dice::roll() {
    this->result = base;
    for (u_short index = 0; index < rolls; index++) {
        if (sides == 0) {
            continue;
        } else {
            this->result += Dice::RandomNumberBetween(1, sides);
        }
    }

    return this->result;
}

/** GETTERS **/
int Dice::getBase() {
    return this->base;
}

u_short Dice::getRolls() {
    return this->rolls;
}

u_short Dice::getSides() {
    return this->sides;
}

int Dice::getResult() {
    return this->result;
}

/** SETTERS **/
Dice* Dice::setBase(int base) {
    this->base = base;

    return this;
}

Dice* Dice::setRolls(u_short rolls) {
    this->rolls = rolls;

    return this;
}

Dice* Dice::setSides(u_short sides) {
    this->sides = sides;

    return this;
}
Dice* Dice::setResult(int result) {
    this->result = result;

    return this;
}
/** SETTERS **/