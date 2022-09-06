#pragma once

#include <ostream>

/// error codes
enum class ecode {
    /// syntax error occurred while parsing input
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