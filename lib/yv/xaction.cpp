#include "include/xaction.hpp"

xaction::xaction(int index, const std::string &identifier)
    : m_index(index),
      m_identifier(identifier) {}

int xaction::index() const {
    return m_index;
}

const std::string &xaction::identifier() const {
    return m_identifier;
}
