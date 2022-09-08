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
    /// the productions in the grammar
    std::vector<std::shared_ptr<gproduction>> m_productions;
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
    /// start symbol
    std::shared_ptr<gsymbol> m_start_symbol;
    /// end symbol
    std::shared_ptr<gsymbol> m_end_symbol;
    /// error symbol
    std::shared_ptr<gsymbol> m_error_symbol;
    /// logger
    glogger m_log;

  public:
    ggrammar();

  public:
    const std::string &identifier() const;
    std::vector<std::shared_ptr<gsymbol>> &symbols();
    const std::shared_ptr<gsymbol>&error_symbol() const;

  public:
    // why ggrammar& returns ?
    // rm `int line` param if not used
    ggrammar &grammar(const std::string &identifier);
    ggrammar &left(int line);
    ggrammar &right(int line);
    ggrammar &none(int line);
    ggrammar &whitespace(int line);
    ggrammar &precedence(int line);
    ggrammar &production(const std::string &identifier, int line);
    ggrammar &end_production();
    ggrammar &error(int line);

  public:
    const std::shared_ptr<gsymbol> &non_terminal_symbol(const std::string &lexeme, int line);

  public:
    const std::shared_ptr<gsymbol> &
    add_symbol(const std::string &lexeme, int line, glexemetype lexeme_type, gsymboltype symbol_type);
    const std::shared_ptr<gproduction> &
    add_production(const std::shared_ptr<gsymbol> &symbol, int line);
};