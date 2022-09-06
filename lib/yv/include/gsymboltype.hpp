#pragma once

/// symbol type
#include <ostream>
enum class gsymboltype {
    SYMBOL_NULL,         ///< null type
    SYMBOL_TERMINAL,     ///< terminal symbol
    SYMBOL_NON_TERMINAL, ///< non-terminal symbol
    SYMBOL_END,          ///< end symbol
    SYMBOL_TYPE_COUNT,   ///< the number of symbol types (useless)
};

inline std::ostream &operator<<(std::ostream &o, const gsymboltype &t) {
    switch (t) {
    case gsymboltype::SYMBOL_NULL:
        o << "SYMBOL_NULL";
        break;
    case gsymboltype::SYMBOL_TERMINAL:
        o << "SYMBOL_TERMINAL";
        break;
    case gsymboltype::SYMBOL_NON_TERMINAL:
        o << "SYMBOL_NON_TERMINAL";
        break;
    case gsymboltype::SYMBOL_END:
        o << "SYMBOL_END";
        break;
    case gsymboltype::SYMBOL_TYPE_COUNT:
        o << "SYMBOL_TYPE_COUNT";
        break;
    }
    return o;
}