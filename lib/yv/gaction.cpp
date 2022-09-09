#include "include/gaction.hpp"
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

void gaction::dump(int separator, const std::string &color) {
    std::string sep(separator, ' ');
    m_log.out << sep << color << "&*this ";
    m_log.out << m_log.cwhite << (void *)&*this << "\n";
    m_log.out << sep << color << ".index ";
    m_log.out << m_log.cwhite << m_index << "\n";
    m_log.out << sep << color << ".ident ";
    m_log.out << m_log.cwhite << m_identifier << "\n";
}
