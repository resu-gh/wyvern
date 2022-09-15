#pragma once

#include "gitem.hpp"
#include "gproduction.hpp"
#include "gsymbol.hpp"
#include "gsymbolc.hpp"
#include <memory>
#include <set>
class gstate {
  private:
    /// state index
    int m_index;
    /// the items that define the positions within
    /// the grammar that this state represents
    std::set<gitem> m_items;

  public:
    gstate();

  public:
    int index() const;
    const std::set<gitem> &items() const;

  public:
    int add_item(const std::shared_ptr<gproduction> &production, int position);
    int add_lookahead_symbols(const std::shared_ptr<gproduction> &production, int position, const std::set<std::shared_ptr<gsymbol>, gsymbolc> &lookahead_symbols);

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};
