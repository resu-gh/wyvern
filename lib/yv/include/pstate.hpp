#pragma once

#include "glogger.hpp"

#include <memory>
#include <vector>

class ptransition;

class pstate {
  public:
    /// state index
    int index;
    /// number of transitions
    int length;
    /// available transitions from this state
    const ptransition* transitions;

  private:
    /// logger
    glogger m_log;

  public:
    pstate();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
