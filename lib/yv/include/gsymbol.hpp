#pragma once

#include "glexemetype.hpp"
#include "gsymbolassoc.hpp"
#include "gsymboltype.hpp"

#include <set>
#include <string>

class gsymbol {
  private:
    std::string m_lexeme;
    std::string m_identifier;
    gsymboltype m_symbol_type;
    glexemetype m_lexeme_type;
    gsymbolassoc m_associativity;
    int m_precedence;
    int m_line;
    int m_index;
    bool m_nullable;

  public:
    gsymbol(const std::string &lexeme);
};