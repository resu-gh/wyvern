#pragma once

#include "gsymbol.hpp"

#include <memory>

class gstate;

class gtransition {
  private:
    /// the symbol that the transition is taken on
    std::shared_ptr<gsymbol> m_symbol;
    /// the state that is transitioned to
    std::shared_ptr<gstate> m_state; // TODO FIXME mutable?
    /// transition index
    mutable int m_index;

  public:
    gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state);

  public:
    int index() const;

  public:
    bool operator<(const gtransition &transition) const;

    static const int INVALID_INDEX = -1; // TODO maybe useless
};
