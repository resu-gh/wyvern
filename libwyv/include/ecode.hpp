#pragma once

#include <ostream>

enum class ecode {
    E_SYNTAX,
};

inline std::ostream &operator<<(std::ostream &o, const ecode &t) {
    switch (t) {
    case ecode::E_SYNTAX:
        o << "E_SYNTAX";
        break;
    }
    return o;
}