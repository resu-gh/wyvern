#pragma once

#include "glexemetype.hpp"
#include "glogger.hpp"
#include "gsymbolassoc.hpp"
#include "gsymboltype.hpp"

#include "gsymbol.hpp" // ?

#include <memory>
#include <vector>

class gsymbol;
class gproduction;

class ggrammar {
  private:
    /// grammar name
    std::string m_identifier;
    /// the symbols in the grammar
    std::vector<std::shared_ptr<gsymbol>> m_symbols;
    /// true iff a whitespace directive is active
    bool m_active_whitespace_directive;
    /// true iff a precedence directive is active
    bool m_active_precedence_directive;
    /// most recently set associativity
    gsymbolassoc m_associativity;
    /// current precedence
    int m_precedence;
    /// currently active production
    std::shared_ptr<gproduction> m_active_production;
    /// currently active symbol
    std::shared_ptr<gsymbol> m_active_symbol;
    /// logger
    glogger m_log;

  public:
    ggrammar();

  public:
    const std::string &identifier() const;

  public:
    // why ggrammar& returns ?
    ggrammar &grammar(const std::string &identifier);
    ggrammar &left(int line);
    ggrammar &right(int line);
    ggrammar &none(int line);
    ggrammar &whitespace(int line);
    ggrammar &precedence(int line);
    ggrammar &production(const std::string &identifier, int line);
    ggrammar &end_production();

  public:
    const std::shared_ptr<gsymbol> &non_terminal_symbol(const std::string &lexeme, int line);

  public:
    const std::shared_ptr<gsymbol> &
    add_symbol(const std::string &lexeme, int line, glexemetype lexeme_type, gsymboltype symbol_type);
};
