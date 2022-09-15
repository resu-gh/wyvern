#include "include/gstate.hpp"
#include "include/gitem.hpp"

#include <cassert>

// TODO INVALID_INDEX -> -1
gstate::gstate()
    : m_index(INVALID_INDEX),
      m_items() {}

int gstate::index() const {
    return m_index;
}

const std::set<gitem> &gstate::items() const {
    return m_items;
}

int gstate::add_item(const std::shared_ptr<gproduction> &production, int position) {
    assert(production.get());
    return m_items.insert(gitem(production, position)).second ? 1 : 0;
}

int gstate::add_lookahead_symbols(const std::shared_ptr<gproduction> &production, int position, const std::set<std::shared_ptr<gsymbol>, gsymbolc> &lookahead_symbols) {
    assert(production.get());
    std::set<gitem>::iterator item = m_items.find(gitem(production, position));
    assert(item != m_items.end());
    return item->add_lookahead_symbols(lookahead_symbols);
}
