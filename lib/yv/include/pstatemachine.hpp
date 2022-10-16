#pragma once

#include "glogger.hpp"
#include "lstatemachine.hpp"
#include "paction.hpp"
#include "pstate.hpp"
#include "psymbol.hpp"
#include "ptransition.hpp"

#include <memory>
#include <string>
#include <vector>

class pstatemachine {
  public:
    /// the identifier of the parser
    const char *identifier;
    /// number of actions
    int actions_size;
    /// the parser actions for this parser state machine
    const paction *actions;
    /// number of symbols
    int symbols_size;
    /// the symbols in the grammar for this parser state machine
    const psymbol *symbols;
    /// the start symbol
    const psymbol *start_symbol;
    /// the end symbol
    const psymbol *end_symbol;
    /// the error symbol
    const psymbol *error_symbol;
    /// number of transitions
    int transitions_size;
    /// the transitions in the state machine for this parser state machine
    const ptransition *transitions;
    /// number of states
    int states_size;
    /// the states in the state machine for this parser state machine
    const pstate *states;
    /// the start state
    const pstate *start_state;
    /// the state machine used by the lexer to match tokens
    const lstatemachine *lexer_state_machine;
    /// the state machine used by the lexer to skip whitespace
    const lstatemachine *whitespace_lexer_state_machine;

  private:
    glogger m_log;

  public:
    pstatemachine();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
