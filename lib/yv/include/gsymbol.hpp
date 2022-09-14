#pragma once

#include "glexemetype.hpp"
#include "glogger.hpp"
#include "gsymbolassoc.hpp"
#include "gsymbolc.hpp"
#include "gsymboltype.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

class gproduction;

/// grammar symbol
class gsymbol : public std::enable_shared_from_this<gsymbol> {
  private:
    /// symbol index (among all symbols)
    int m_index;
    /// the line that this symbol is defined on
    int m_line;
    /// symbol lexeme
    std::string m_lexeme;
    /// symbol identifier (generated from lexeme)
    std::string m_identifier;
    /// symbol type (terminal, non-terminal or end)
    gsymboltype m_symbol_type;
    /// symbol's lexeme type (regex or literal)
    glexemetype m_lexeme_type;
    /// symbol associativity
    gsymbolassoc m_associativity;
    /// symbol precedence
    int m_precedence;
    /// true if symbol is nullable, otherwise false
    bool m_nullable;
    /// the productions that reduce to this symbol
    std::vector<std::shared_ptr<gproduction>> m_productions;
    /// the symbols that can start this symbol in a production or regex
    std::set<std::shared_ptr<gsymbol>, gsymbolc> m_first;
    /// the symbols that can follow this symbol in a production or regex
    std::set<std::shared_ptr<gsymbol>, gsymbolc> m_follow;
    /// logger
    glogger m_log;

  public:
    gsymbol(const std::string &lexeme);

  public:
    int index() const;
    int line() const;
    const std::string &lexeme() const;
    const std::string &identifier() const;
    gsymboltype symbol_type() const;
    glexemetype lexeme_type() const;
    gsymbolassoc associativity() const;
    int precedence() const;
    bool nullable() const;
    const std::vector<std::shared_ptr<gproduction>> &productions() const;
    const std::set<std::shared_ptr<gsymbol>, gsymbolc> &first() const;
    const std::set<std::shared_ptr<gsymbol>, gsymbolc> &follow() const;

  public:
    void set_index(int index);
    void set_line(int line);
    void set_symbol_type(gsymboltype symbol_type);
    void set_lexeme_type(glexemetype lexeme_type);
    void set_associativity(gsymbolassoc associativity);
    void set_precedence(int precedence);

  public:
    bool matches(const std::string &lexeme, gsymboltype symbol_type);
    void append_production(const std::shared_ptr<gproduction> &production);
    void calculate_identifier();

    std::shared_ptr<gsymbol> implicit_terminal() const;                                // TODO FIXME ?
    void replace_by_non_terminal(const std::shared_ptr<gsymbol> &non_terminal_symbol); // TODO FIXME?
    int calculate_first();
    int add_symbol_to_first(const std::shared_ptr<gsymbol> &symbol);
    int add_symbols_to_first(const std::set<std::shared_ptr<gsymbol>, gsymbolc> &symbols);
    int calculate_follow();
    int add_symbol_to_follow(const std::shared_ptr<gsymbol> &symbol);
    int add_symbols_to_follow(const std::set<std::shared_ptr<gsymbol>, gsymbolc> &symbols);

  public:
    std::string microdump() const;

    std::shared_ptr<gsymbol> self() {
        return shared_from_this();
    }
};
