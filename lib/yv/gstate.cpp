#include "include/gstate.hpp"
#include "include/gitem.hpp"
#include "include/gtransition.hpp"

#include <cassert>

// TODO INVALID_INDEX -> -1
gstate::gstate()
    : m_index(INVALID_INDEX),
      m_processed(false),
      m_items(),
      m_transitions() {}

int gstate::index() const {
    return m_index;
}

bool gstate::processed() const {
    return m_processed;
}

const std::set<gitem> &gstate::items() const {
    return m_items;
}

void gstate::set_index(int index) {
    m_index = index;
}

void gstate::set_processed(bool processed) {
    m_processed = processed;
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

void gstate::add_transition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state) {
    assert(symbol.get());
    assert(state.get());
    assert(m_transitions.find(gtransition(symbol, state)) == m_transitions.end());
    m_transitions.insert(gtransition(symbol, state));
}
