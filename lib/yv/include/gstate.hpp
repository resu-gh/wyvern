#pragma once

#include "gitem.hpp"
#include "gproduction.hpp"
#include "gsymbol.hpp"
#include "gsymbolc.hpp"
#include "gtransition.hpp"

#include <memory>
#include <set>

class gstate {
  private:
    /// state index
    int m_index;
    /// true if this state has been processed during state machine generation
    bool m_processed;
    /// the items that define the positions within
    /// the grammar that this state represents
    std::set<gitem> m_items;
    /// the available transitions from this state
    std::set<gtransition> m_transitions;

  public:
    gstate();

  public:
    int index() const;
    bool processed() const;
    const std::set<gitem> &items() const;

  public:
    void set_index(int index);
    void set_processed(bool processed);

  public:
    int add_item(const std::shared_ptr<gproduction> &production, int position);
    int add_lookahead_symbols(const std::shared_ptr<gproduction> &production, int position, const std::set<std::shared_ptr<gsymbol>, gsymbolc> &lookahead_symbols);
    void add_transition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state);

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};
