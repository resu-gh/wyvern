#pragma once

#include <ostream>

/// symbol lexeme type
enum class glexemetype {
    /// null type
    LEXEME_NULL,
    /// literal
    LEXEME_LITERAL,
    /// regex
    LEXEME_REGULAR_EXPRESSION,
    /// number of lexeme types (useless)
    LEXEME_TYPE_COUNT,
};

inline std::ostream &operator<<(std::ostream &o, const glexemetype &t) {
    switch (t) {
    case glexemetype::LEXEME_NULL:
        o << "LNULL";
        break;
    case glexemetype::LEXEME_LITERAL:
        o << "LLITERAL";
        break;
    case glexemetype::LEXEME_REGULAR_EXPRESSION:
        o << "LREGEX";
        break;
    case glexemetype::LEXEME_TYPE_COUNT:
        o << "LEXEME_TYPE_COUNT";
        break;
    }
    return o;
}
