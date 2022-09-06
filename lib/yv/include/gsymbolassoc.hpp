#pragma once

#include <ostream>

/// symbol associativity
enum class gsymbolassoc {
    ASSOCIATE_NULL,  ///< undefined associativity
    ASSOCIATE_NONE,  ///< no associativity
    ASSOCIATE_LEFT,  ///< left associativity
    ASSOCIATE_RIGHT, ///< right associativity
};

inline std::ostream &operator<<(std::ostream &o, const gsymbolassoc &t) {
    switch (t) {
    case gsymbolassoc::ASSOCIATE_NULL:
        o << "ASSOCIATE_NULL";
        break;
    case gsymbolassoc::ASSOCIATE_NONE:
        o << "ASSOCIATE_NONE";
        break;
    case gsymbolassoc::ASSOCIATE_LEFT:
        o << "ASSOCIATE_LEFT";
        break;
    case gsymbolassoc::ASSOCIATE_RIGHT:
        o << "ASSOCIATE_RIGHT";
        break;
    }
    return o;
}