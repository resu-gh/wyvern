#include "include/gtransition.hpp"
#include "include/gstate.hpp"

#include <cassert>

// TODO INVALID_INDEX -> -1
gtransition::gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state)
    : m_symbol(symbol),
      m_state(state),
      m_index(INVALID_INDEX) {}

int gtransition::index() const {
    return m_index;
}

bool gtransition::operator<(const gtransition &transition) const {
    assert(m_symbol->index() >= 0);
    assert(transition.m_symbol->index() >= 0);
    return m_symbol->index() < transition.m_symbol->index();
}
