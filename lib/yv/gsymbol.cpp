#include "include/gsymbol.hpp"
#include "include/gsymboltype.hpp"

#include <cassert>

gsymbol::gsymbol(const std::string &lexeme)
    : m_index(-1),
      m_line(0),
      m_lexeme(lexeme),
      m_symbol_type(gsymboltype::SYMBOL_NULL),
      m_lexeme_type(glexemetype::LEXEME_NULL) {}

int gsymbol::index() const {
    return m_index;
}

const std::string &gsymbol::lexeme() const {
    return m_lexeme;
}

gsymboltype gsymbol::symbol_type() const {
    return m_symbol_type;
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

bool gsymbol::matches(const std::string &lexeme, gsymboltype symbol_type) {
    return m_lexeme == lexeme && m_symbol_type == symbol_type;
}
