#pragma once

#include <ostream>

/// symbol associativity
enum class gsymbolassoc {
    /// undefined associativity
    ASSOCIATE_NULL,
    /// no associativity
    ASSOCIATE_NONE,
    /// left associativity
    ASSOCIATE_LEFT,
    /// right associativity
    ASSOCIATE_RIGHT
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