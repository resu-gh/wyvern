#pragma once

#include "gproduction.hpp"
#include "gsymbol.hpp"
#include "gsymbolc.hpp"
#include <memory>
#include <set>
class gitem {
  private:
    /// the position of the `dot` (cursor) in this item
    int m_position;
    /// the production that this item is for
    std::shared_ptr<gproduction> m_production;
    /// the lookahead symbols for this item
    mutable std::set<std::shared_ptr<gsymbol>, gsymbolc> m_lookahead_symbols;

  public:
    gitem(); // TODO maybe useless
    gitem(const std::shared_ptr<gproduction> &production, int position);

  public:
    int position() const;
    const std::shared_ptr<gproduction> &production() const;
    const std::set<std::shared_ptr<gsymbol>, gsymbolc>& lookahead_symbols() const;

  public:
    int add_lookahead_symbols(const std::set<std::shared_ptr<gsymbol>, gsymbolc> &lookahead_symbols) const;

  public:
    bool operator<(const gitem &item) const;
};
