#pragma once

#include "glexemetype.hpp"
#include "gsymbolassoc.hpp"
#include "gsymboltype.hpp"

#include <memory>
#include <string>
#include <vector>

class gproduction;

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
    /// symbol associativity
    gsymbolassoc m_associativity;
    /// symbol precedence
    int m_precedence;
    /// the productions that reduce to this symbol
    std::vector<std::shared_ptr<gproduction>> m_productions;

  public:
    gsymbol(const std::string &lexeme);

  public:
    int index() const;
    int line() const;
    const std::string &lexeme() const;
    gsymboltype symbol_type() const;
    glexemetype lexeme_type() const;
    gsymbolassoc associativity() const;
    int precedence() const;
    const std::vector<std::shared_ptr<gproduction>> &productions() const;

  public:
    void set_line(int line);
    void set_symbol_type(gsymboltype symbol_type);
    void set_lexeme_type(glexemetype lexeme_type);
    void set_associativity(gsymbolassoc associativity);
    void set_precedence(int precedence);

  public:
    bool matches(const std::string &lexeme, gsymboltype symbol_type);
    void append_production(const std::shared_ptr<gproduction> &production);
};
