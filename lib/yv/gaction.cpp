#include "include/gaction.hpp"
#include <sstream>
#include <string>

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

std::string gaction::microdump() const {
    std::stringstream s;
    s << (void*)&*this << " ";
    s << m_index << " ";
    s << m_identifier << " ";
    return s.str();
}
