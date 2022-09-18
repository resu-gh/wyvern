#pragma once

#include <ostream>

/// token type
/// increasing priority ordering
/// a state that can match both literal and regex
/// will match a literal
enum class xtokentype {
    TOKEN_NULL,
    TOKEN_REGULAR_EXPRESSION,
    TOKEN_LITERAL,
};

inline std::ostream &operator<<(std::ostream &o, const xtokentype &t) {
    switch (t) {
    case xtokentype::TOKEN_NULL:
        o << "TNULL";
        break;
    case xtokentype::TOKEN_REGULAR_EXPRESSION:
        o << "TREGEX";
        break;
    case xtokentype::TOKEN_LITERAL:
        o << "TLITERAL";
        break;
    }
    return o;
}
