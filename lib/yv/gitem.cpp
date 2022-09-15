#include "include/gitem.hpp"

#include <cassert>
#include <cstddef>

// TODO maybe useless
gitem::gitem()
    : m_position(0),
      m_production(),
      m_lookahead_symbols() {}

gitem::gitem(const std::shared_ptr<gproduction> &production, int position)
    : m_position(position),
      m_production(production),
      m_lookahead_symbols() {
    assert(m_production.get());
    assert(m_position >= 0 && m_position < m_production->length() + 1);
}

int gitem::position() const {
    return m_position;
}

const std::shared_ptr<gproduction> &gitem::production() const {
    return m_production;
}

const std::set<std::shared_ptr<gsymbol>, gsymbolc> &gitem::lookahead_symbols() const {
    return m_lookahead_symbols;
}

int gitem::add_lookahead_symbols(const std::set<std::shared_ptr<gsymbol>, gsymbolc> &lookahead_symbols) const {
    std::size_t original_size = m_lookahead_symbols.size();
    m_lookahead_symbols.insert(lookahead_symbols.begin(), lookahead_symbols.end());
    return int(m_lookahead_symbols.size() - original_size);
}

bool gitem::operator<(const gitem &item) const {
    return m_production->index() < item.m_production->index() || (m_production->index() == item.m_production->index() && m_position < item.m_position);
}
