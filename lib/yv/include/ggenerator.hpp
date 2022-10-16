#pragma once

#include "gaction.hpp"
#include "glogger.hpp"
#include "gstate.hpp"
#include "gstatec.hpp"
#include "gsymbol.hpp"

#include <iostream>
#include <memory>
#include <set>
#include <vector>

class ggrammar;

/// @brief parser state machine generator

class ggenerator {
  private:
    /// grammar
    std::shared_ptr<ggrammar> m_grammar;
    /// parser identifier
    std::string m_identifier;
    /// actions in the parser
    std::vector<std::shared_ptr<gaction>> m_actions;
    /// productions in the parser
    std::vector<std::shared_ptr<gproduction>> m_productions;
    /// symbols in the parser
    std::vector<std::shared_ptr<gsymbol>> m_symbols;
    /// start state
    std::shared_ptr<gsymbol> m_start_symbol;
    /// end state
    std::shared_ptr<gsymbol> m_end_symbol;
    /// error state
    std::shared_ptr<gsymbol> m_error_symbol;
    /// the states in the parser's state machine
    std::set<std::shared_ptr<gstate>, gstatec> m_states;
    /// the start state
    std::shared_ptr<gstate> m_start_state;
    /// number of errors occurred during parsing and generator
    int m_errors;
    /// logger
    glogger m_log;

    /// constructors
  public:
    ggenerator();

    /// main
  public:
    int generate(const std::shared_ptr<ggrammar> &);

    /// getters
  public:
    const std::vector<std::shared_ptr<gaction>> &actions() const;
    const std::vector<std::shared_ptr<gsymbol>> &symbols() const;
    const std::set<std::shared_ptr<gstate>, gstatec> &states() const;
    const std::shared_ptr<gstate> &start_state() const;

    /// tranformations
  private:
    void calculate_identifiers();
    void calculate_terminal_and_non_terminal_symbols();
    void calculate_implicit_terminal_symbols();
    void calculate_symbol_indices();
    void calculate_first();
    void calculate_follow();
    void calculate_precedence_of_productions();

    /// checks
  private:
    void check_for_undefined_symbol_errors();
    void check_for_unreferenced_symbol_errors();
    void check_for_error_symbol_on_left_hand_side_errors();

    /// operations
  private:
    void replace_references_to_symbol(const std::shared_ptr<gsymbol> &to_symbol, const std::shared_ptr<gsymbol> &with_symbol);
    void clojure(const std::shared_ptr<gstate> &state);
    int lookahead_clojure(const std::shared_ptr<gstate> &state) const;
    std::shared_ptr<gstate> goto_(const std::shared_ptr<gstate> &state, const gsymbol &symbol);
    int lookahead_goto(const std::shared_ptr<gstate> &state) const;
    std::set<std::shared_ptr<gsymbol>, gsymbolc> lookahead(const gitem &item) const;

    /// generation
  private:
    void generate_states();
    void generate_indices_for_states();
    void generate_reduce_transitions();
    void generate_reduce_transition(const std::shared_ptr<gstate> &state, const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gproduction> &production);
    void generate_indices_for_transitions();

    /// debug
  public:
    void dump(bool compact) const;
};
