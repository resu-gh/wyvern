#pragma once

#include "ggenerator.hpp"
#include "glogger.hpp"
#include "paction.hpp"
#include "pstate.hpp"
#include "pstatemachine.hpp"
#include "psymbol.hpp"
#include "xcompiler.hpp"

#include <array>
#include <deque>
#include <memory>
#include <string>
#include <vector>

class ggrammar;

class gcompiler {
  private:
    /// grammar
    std::shared_ptr<ggrammar> m_grammar;
    /// generator
    std::shared_ptr<ggenerator> m_generator;
    /// processed strings
    std::deque<std::string> m_strings;

    /// the parser actions for this parser state machine
    std::unique_ptr<paction[]> m_actions;
    /// the symbols in the grammar for this parser state machine
    std::unique_ptr<psymbol[]> m_symbols;
    /// the transitions in the state machine for this parser state machine
    std::unique_ptr<ptransition[]> m_transitions;
    /// the states in the state machine for this parser state machine
    std::unique_ptr<pstate[]> m_states;
    /// allocated lexer state machine
    std::unique_ptr<xcompiler> m_lexer;
    /// allocated whitespace lexer state machine;
    std::unique_ptr<xcompiler> m_whitespace_lexer;
    /// allocated parser state machine, TODO unique_ptr
    std::unique_ptr<pstatemachine> m_parser_state_machine;

    /// logger
    glogger m_log;

  public:
    gcompiler();

  public:
    int compile(std::string::iterator &, std::string::iterator &);

  public:
    const std::unique_ptr<pstatemachine> &parser_state_machine() const;

  private:
    void set_actions(std::unique_ptr<paction[]> &actions, int actions_size);
    void set_symbols(std::unique_ptr<psymbol[]> &symbols, int symbols_size);
    void set_transitions(std::unique_ptr<ptransition[]> &transitions, int transitions_size);
    void set_states(std::unique_ptr<pstate[]> &states, int states_size, const pstate *start_state);

  private:
    const char *add_string(const std::string &string);
    void populate_parser_state_machine();
    void populate_lexer_state_machine();
    // void populate_whitespace_lexer_state_machine();
};
