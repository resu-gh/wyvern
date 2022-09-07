#pragma once

#include <ostream>

/// symbol type
enum class gsymboltype {
    /// null type
    SYMBOL_NULL,
    /// terminal
    SYMBOL_TERMINAL,
    /// non-terminal
    SYMBOL_NON_TERMINAL,
    /// end symbol
    SYMBOL_END,
    /// number of symbol types (useless)
    SYMBOL_TYPE_COUNT
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