#pragma once

#include "glogger.hpp"

#include <memory>
#include <vector>

class ptransition;

class pstate {
  public:
    /// state index
    int index;
    /// available transitions from this state
    std::vector<ptransition>::const_iterator transitions_begin;
    std::vector<ptransition>::const_iterator transitions_end;

  private:
    /// logger
    glogger m_log;

  public:
    pstate();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
