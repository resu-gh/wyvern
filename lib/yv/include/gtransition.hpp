#pragma once

#include "glogger.hpp"
#include "gsymbol.hpp"
#include "gtranstype.hpp"

#include <memory>
#include <string>

class gstate;

class gtransition {
  private:
    /// the symbol that the transition is taken on
    std::shared_ptr<gsymbol> m_symbol;
    /// the state that is transitioned to
    std::shared_ptr<gstate> m_state; // TODO FIXME mutable?
    /// transition index
    mutable int m_index;
    /// transition type
    mutable gtranstype m_type;
    /// logger
    glogger m_log;

  public:
    gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state);

  public:
    int index() const;
    gtranstype type() const;
    const std::shared_ptr<gsymbol> &symbol() const;
    const std::shared_ptr<gstate> &state() const;

  public:
    bool taken_on_symbol(const std::shared_ptr<gsymbol> &symbol) const;


  public:
    bool operator<(const gtransition &transition) const;

  public:
    std::string microdump() const;
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};
