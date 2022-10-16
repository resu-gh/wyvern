#pragma once

#include "glogger.hpp"
#include "gtranstype.hpp"

#include <memory>

class psymbol;
class pstate;

class ptransition {
  public:
    /// the symbol that the transition is taken on
    const psymbol *symbol;
    /// the state that is transitioned to
    const pstate *state;
    /// the symbol that is reduced to (or null if this isn't a reducing transition)
    const psymbol *reduced_symbol;
    /// the number of symbols on the rhs of the reduced production
    int reduced_length;
    /// the precedence of the reduce production (or 0 for the default precedence or no
    /// reduction)
    int precedence;
    /// the index of the action taken on reduce (or gaction::INVALID_INDEX if there is no action associated with the reduce)
    int action;
    /// the type of this transition
    gtranstype type;
    /// the index of this transition
    int index;

  private:
    /// logger
    glogger m_log;

  public:
    ptransition();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
