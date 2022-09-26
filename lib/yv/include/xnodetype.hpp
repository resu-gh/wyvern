#pragma once

/// the type of a node in a parsed regex
#include <ostream>
enum class xnodetype {
    LEXER_NODE_NULL,
    LEXER_NODE_CAT,
    LEXER_NODE_OR,
    LEXER_NODE_STAR,
    LEXER_NODE_PLUS,
    LEXER_NODE_OPTIONAL,
    LEXER_NODE_SYMBOL,
    LEXER_NODE_ACTION,
    LEXER_NODE_COUNT,
};

inline std::ostream &operator<<(std::ostream &o, const xnodetype &t) {
    switch (t) {
    case xnodetype::LEXER_NODE_NULL:
        o << "ND_NULL";
        break;
    case xnodetype::LEXER_NODE_CAT:
        o << "ND_CAT";
        break;
    case xnodetype::LEXER_NODE_OR:
        o << "ND_OR";
        break;
    case xnodetype::LEXER_NODE_STAR:
        o << "ND_STAR";
        break;
    case xnodetype::LEXER_NODE_PLUS:
        o << "ND_PLUS";
        break;
    case xnodetype::LEXER_NODE_OPTIONAL:
        o << "ND_OPT";
        break;
    case xnodetype::LEXER_NODE_SYMBOL:
        o << "ND_SYM";
        break;
    case xnodetype::LEXER_NODE_ACTION:
        o << "ND_ACT";
        break;
    case xnodetype::LEXER_NODE_COUNT:
        o << "ND_COUNT";
        break;
    }
    return o;
}
