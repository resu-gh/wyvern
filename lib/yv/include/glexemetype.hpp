#pragma once

#include <ostream>

/// lexeme type
enum class glexemetype {
    LEXEME_NULL,       ///< null type
    LEXEME_LITERAL,    ///< literal type
    LEXEME_REGEX,      ///< regex type
    LEXEME_TYPE_COUNT, ///< number of lexeme types (useless)
};

inline std::ostream &operator<<(std::ostream &o, const glexemetype &t) {
    switch (t) {
    case glexemetype::LEXEME_NULL:
        o << "LEXEME_NULL";
        break;
    case glexemetype::LEXEME_LITERAL:
        o << "LEXEME_LITERAL";
        break;
    case glexemetype::LEXEME_REGEX:
        o << "LEXEME_REGEX";
        break;
    case glexemetype::LEXEME_TYPE_COUNT:
        o << "LEXEME_TYPE_COUNT";
        break;
    }
    return o;
}