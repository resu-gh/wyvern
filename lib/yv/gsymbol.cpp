#include "include/gsymbol.hpp"
#include "include/gsymboltype.hpp"

#include <cassert>

gsymbol::gsymbol(const std::string &lexeme)
    : m_index(-1),
      m_line(0),
      m_lexeme(lexeme),
      m_symbol_type(gsymboltype::SYMBOL_NULL),
      m_lexeme_type(glexemetype::LEXEME_NULL),
      m_associativity(gsymbolassoc::ASSOCIATE_NULL),
      m_precedence(0) {}

int gsymbol::index() const {
    return m_index;
}

const std::string &gsymbol::lexeme() const {
    return m_lexeme;
}

gsymboltype gsymbol::symbol_type() const {
    return m_symbol_type;
}

gsymbolassoc gsymbol::associativity() const {
    return m_associativity;
}

int gsymbol::precedence() const {
    return m_precedence;
}

void gsymbol::set_line(int line) {
    assert(line >= 0);
    m_line = line;
}

void gsymbol::set_symbol_type(gsymboltype symbol_type) {
    assert(symbol_type >= gsymboltype::SYMBOL_NULL && m_symbol_type < gsymboltype::SYMBOL_TYPE_COUNT);
    m_symbol_type = symbol_type;
}

void gsymbol::set_lexeme_type(glexemetype lexeme_type) {
    assert(lexeme_type >= glexemetype::LEXEME_NULL && lexeme_type < glexemetype::LEXEME_TYPE_COUNT);
    m_lexeme_type = lexeme_type;
}

void gsymbol::set_associativity(gsymbolassoc associativity) {
    assert(associativity >= gsymbolassoc::ASSOCIATE_NONE && associativity <= gsymbolassoc::ASSOCIATE_RIGHT);
    m_associativity = associativity;
}

void gsymbol::set_precedence(int precedence) {
    assert(precedence >= 0);
    m_precedence = precedence;
}

bool gsymbol::matches(const std::string &lexeme, gsymboltype symbol_type) {
    return m_lexeme == lexeme && m_symbol_type == symbol_type;
}
