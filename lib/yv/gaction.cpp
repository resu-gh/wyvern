#include "include/gaction.hpp"

#include <cassert>

gaction::gaction(int index, const std::string &identifier)
    : m_index(index),
      m_identifier(identifier) {
    assert(m_index >= 0);
    assert(!m_identifier.empty());
}

int gaction::index() const {
    return m_index;
}

const std::string &gaction::identifier() const {
    return m_identifier;
}