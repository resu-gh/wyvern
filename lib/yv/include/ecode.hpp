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
    /// the 'error' symbol has been used on the lhs of a production
    E_PARSER_ERROR_ON_LHS,
    /// a `shift-reduce` or `reduce-reduce` conflict was found in the parse table
    E_PARSER_PARSE_TABLE_CONFLICT,
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
    case ecode::E_PARSER_ERROR_ON_LHS:
        o << "E_PARSER_ERROR_ON_LHS";
        break;
    case ecode::E_PARSER_PARSE_TABLE_CONFLICT:
        o << "E_PARSER_PARSE_TABLE_CONFLICT";
        break;
    }
    return o;
}
