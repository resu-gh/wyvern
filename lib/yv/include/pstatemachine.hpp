#pragma once

#include "glogger.hpp"
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
    std::string identifier;

    /// the parser actions for this parser state machine
    std::vector<paction>::const_iterator actions_begin;
    std::vector<paction>::const_iterator actions_end;

    /// the symbols in the grammar for this parser state machine
    std::vector<psymbol>::const_iterator symbols_begin;
    std::vector<psymbol>::const_iterator symbols_end;

    // std::weak_ptr<psymbol> start_symbol;
    // std::weak_ptr<psymbol> end_symbol;
    // std::weak_ptr<psymbol> error_symbol;

    /// the transitions in the state machine for this parser state machine
    std::vector<ptransition>::const_iterator transitions_begin;
    std::vector<ptransition>::const_iterator transitions_end;

    /// the states in the state machine for this parser state machine
    std::vector<pstate>::const_iterator states_begin;
    std::vector<pstate>::const_iterator states_end;

    // std::weak_ptr<pstate> start_state;

  private:
    glogger m_log;

  public:
    pstatemachine();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
