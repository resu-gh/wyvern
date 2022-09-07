#pragma once

#include "glexemetype.hpp"
#include "gsymboltype.hpp"

#include <string>

/// grammar symbol
class gsymbol {
  private:
    /// symbol index (among all symbols)
    int m_index;
    /// the line that this symbol is defined on
    int m_line;
    /// symbol lexeme
    std::string m_lexeme;
    /// symbol type (terminal, non-terminal or end)
    gsymboltype m_symbol_type;
    /// symbol's lexeme type (regex or literal)
    glexemetype m_lexeme_type;

  public:
    gsymbol(const std::string &lexeme);

  public:
    int index() const;
    const std::string &lexeme() const;
    gsymboltype symbol_type() const;

  public:
    void set_line(int line);
    void set_symbol_type(gsymboltype symbol_type);
    void set_lexeme_type(glexemetype lexeme_type);

  public:
    bool matches(const std::string &lexeme, gsymboltype symbol_type);
};