#pragma once

#include "ggenerator.hpp"
#include "glogger.hpp"
#include "paction.hpp"
#include "pstate.hpp"
#include "pstatemachine.hpp"
#include "psymbol.hpp"

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

    /// allocated parser state machine, TODO unique_ptr
    std::shared_ptr<pstatemachine> m_parser_state_machine;
    /// the parser actions for this parser state machine
    std::shared_ptr<std::vector<paction>> m_actions;
    /// the symbols in the grammar for this parser state machine
    std::shared_ptr<std::vector<psymbol>> m_symbols;
    /// the transitions in the state machine for this parser state machine
    std::shared_ptr<std::vector<ptransition>> m_transitions;
    /// the states in the state machine for this parser state machine
    std::shared_ptr<std::vector<pstate>> m_states;

    /// logger
    glogger m_log;

  public:
    gcompiler();

  public:
    int compile(std::string::iterator &, std::string::iterator &);

  public:
    const std::shared_ptr<pstatemachine> &parser_state_machine() const;

  private:
    void set_actions(std::shared_ptr<std::vector<paction>> &actions, int actions_size);
    void set_symbols(std::shared_ptr<std::vector<psymbol>> &symbols, int symbols_size);
    void set_transitions(std::shared_ptr<std::vector<ptransition>> &transitions, int transitions_size);
    void set_states(std::shared_ptr<std::vector<pstate>> &states, int states_size);

  private:
    const std::string &add_string(const std::string &string);
    void populate_parser_state_machine();
};
