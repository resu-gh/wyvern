#pragma once

#include <ostream>

/// error codes
enum class ecode {
    /// syntax error occurred while parsing input
    E_SYNTAX,
    /// grammar symbol is referenced but undefined
    E_PARSER_UNDEF_SYMBOL,
    /// grammar symbol is defined but referenced
    E_PARSER_UNREF_SYMBOL,
};

inline std::ostream &operator<<(std::ostream &o, const ecode &t) {
    switch (t) {
    case ecode::E_SYNTAX:
        o << "E_SYNTAX";
        break;
    case ecode::E_PARSER_UNDEF_SYMBOL:
        o << "E_PARSER_UNDEF_SYMBOL";
        break;
    case ecode::E_PARSER_UNREF_SYMBOL:
        o << "E_PARSER_UNREF_SYMBOL";
        break;
    }
    return o;
}
