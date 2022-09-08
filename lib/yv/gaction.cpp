#include "include/gaction.hpp"

gaction::gaction(int index, const std::string &identifier)
    : m_index(index),
      m_identifier(identifier),
      m_log() {}

int gaction::index() const {
    return m_index;
}

const std::string &gaction::identifier() const {
    return m_identifier;
}
