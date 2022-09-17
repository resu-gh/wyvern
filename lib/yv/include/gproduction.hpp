#pragma once

#include "gaction.hpp"
#include "glogger.hpp"
#include "gsymbol.hpp"

#include <memory>
#include <vector>

/// production entity
/// defines a symbol on the lhs that is reduced
// from 0 or more nodes on the rhs
class gproduction {
  private:
    /// production index
    int m_index;
    /// lhs symbol of this production
    std::shared_ptr<gsymbol> m_symbol;
    /// the line where this production is defined
    int m_line;
    /// the column where this production is defined
    int m_column;
    /// the action to take when this production is reduced (null = no action)
    std::shared_ptr<gaction> m_action;
    /// rhs symbols of this production
    std::vector<std::shared_ptr<gsymbol>> m_symbols;
    /// the symbol that defines precedence for this production
    std::shared_ptr<gsymbol> m_precedence_symbol;
    /// logger
    glogger m_log;

  public:
    gproduction(int index, const std::shared_ptr<gsymbol> &symbol, int line, int column, const std::shared_ptr<gaction> &action);

  public:
    int index() const;
    const std::shared_ptr<gsymbol> &symbol() const;
    int line() const;
    int column() const;
    const std::shared_ptr<gaction> &action() const;
    const std::vector<std::shared_ptr<gsymbol>> &symbols() const;
    const std::shared_ptr<gsymbol> &precedence_symbol() const;

  public:
    void set_precedence_symbol(const std::shared_ptr<gsymbol> &symbol);
    void set_action(const std::shared_ptr<gaction> &action);

  public:
    int length() const;
    void append_symbol(const std::shared_ptr<gsymbol> &symbol);
    int count_references_to_symbol(const std::shared_ptr<gsymbol> &symbol);
    void replace_references_to_symbol(const std::shared_ptr<gsymbol> &to_symbol, const std::shared_ptr<gsymbol> &with_symbol);
    int precedence() const;
    std::shared_ptr<gsymbol> find_rightmost_terminal_symbol() const;
    std::shared_ptr<gsymbol> symbol_by_position(int position) const;

  public:
    std::string microdump() const;
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;

  public:
    static const int INVALID_INDEX = -1;
};
