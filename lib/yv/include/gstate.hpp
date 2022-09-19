#pragma once

#include "gitem.hpp"
#include "glogger.hpp"
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
    /// logger
    glogger m_log;

  public:
    gstate();

  public:
    int index() const;
    bool processed() const;
    const std::set<gitem> &items() const;
    const std::set<gtransition> &transitions() const;

  public:
    void set_index(int index);
    void set_processed(bool processed);

  public:
    int add_item(const std::shared_ptr<gproduction> &production, int position);
    int add_lookahead_symbols(const std::shared_ptr<gproduction> &production, int position, const std::set<std::shared_ptr<gsymbol>, gsymbolc> &lookahead_symbols);
    void add_transition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state);
    
    // TODO custom impl
    std::set<gtransition>::iterator find_transition_by_symbol(const std::shared_ptr<gsymbol> &symbol);

  public:
    bool operator<(const gstate &state) const;

  public:
    std::string microdump() const;
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};
