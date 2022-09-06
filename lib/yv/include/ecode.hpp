#pragma once

#include <ostream>

/// error codes
enum class ecode {
    E_SYNTAX, ///< syntax error occurred while parsing input
};

inline std::ostream &operator<<(std::ostream &o, const ecode &t) {
    switch (t) {
    case ecode::E_SYNTAX:
        o << "E_SYNTAX";
        break;
    }
    return o;
}