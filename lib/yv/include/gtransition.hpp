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
    mutable std::shared_ptr<gstate> m_state; // TODO FIXME mutable?
    /// the symbol that is reduced to or nullptr if this isn't a reducing transition
    mutable std::shared_ptr<gsymbol> m_reduced_symbol; // TODO FIXME mutable?
    /// transition index
    mutable int m_index;
    /// the precedence of the reduce production or 0 for the default precedence or no reduction
    mutable int m_precedence;
    /// the index of the action taken on reduce or gaction::INVALID_INDEX if there
    /// is no action associated with the reduce
    mutable int m_action;
    /// the number of symbols on the rhs of the reduced production
    mutable int m_reduced_length;
    /// transition type
    mutable gtranstype m_type;
    /// logger
    glogger m_log;

  public:
    gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state);
    gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gsymbol> &reduced_symbol, int reduced_length, int precedence, int action);

  public:
    int index() const;
    int precedence() const;
    int action() const;
    int reduced_length() const;
    gtranstype type() const;
    const std::shared_ptr<gsymbol> &symbol() const;
    const std::shared_ptr<gstate> &state() const;
    const std::shared_ptr<gsymbol> &reduced_symbol() const;

  public:
    void set_index(int index) const;

  public:
    bool taken_on_symbol(const std::shared_ptr<gsymbol> &symbol) const;
    void override_shift_to_reduce(const std::shared_ptr<gsymbol> &symbol, int length, int precedence, int action) const;
    void override_reduce_to_reduce(const std::shared_ptr<gsymbol> &symbol, int length, int precedence, int action) const;

  public:
    bool operator<(const gtransition &transition) const;

  public:
    std::string microdump() const;
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};
