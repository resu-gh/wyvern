#include "include/gaction.hpp"
#include <sstream>
#include <string>

gaction::gaction(int index, const std::string &identifier)
    : m_index(index),
      m_identifier(identifier),
      m_log("yyv", "gacti", 255) {}

int gaction::index() const {
    return m_index;
}

const std::string &gaction::identifier() const {
    return m_identifier;
}

std::string gaction::microdump() const {
    std::stringstream s;
    s << (void *)&*this << " ";
    s << m_index << " ";
    s << m_identifier << " ";
    return s.str();
}

void gaction::json(int sc, bool nested, int in, bool inlined, int uc) {
    m_log.out << m_log.chl << m_log.sp(in) << "gacti";
    m_log.out << m_log.cnr << (inlined ? ": { " : ": {\n");

    if (uc) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "use_count: ";
        m_log.out << m_log.chl << uc;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "this: ";
    m_log.out << m_log.chl << &*this;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "index: ";
    m_log.out << m_log.chl << m_index;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "identifier: ";
    m_log.out << m_log.chl << m_identifier;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
