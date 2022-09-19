#include "include/gitem.hpp"

#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>

// TODO maybe useless
gitem::gitem()
    : m_position(0),
      m_production(),
      m_lookahead_symbols(),
      m_log("yyv", "gitem", 0) {}

gitem::gitem(const std::shared_ptr<gproduction> &production, int position)
    : m_position(position),
      m_production(production),
      m_lookahead_symbols(),
      m_log("yyv", "gitem", 0) {
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

bool gitem::next_node(const gsymbol &symbol) const {
    return &*m_production->symbol_by_position(m_position) == &symbol;
}

bool gitem::dot_at_end() const {
    return m_position == m_production->length();
}

bool gitem::operator<(const gitem &item) const {
    return m_production->index() < item.m_production->index() || (m_production->index() == item.m_production->index() && m_position < item.m_position);
}

std::string gitem::microdump() const {
    std::stringstream s;
    s << &*this << " ";
    s << m_position << " prod ";
    s << m_production->microdump();
    s << "lookah[" << m_lookahead_symbols.size() << "] ";
    return s.str();
}

void gitem::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "gitem";
    m_log.out << m_log.cnr << (inlined ? ": { " : ": {\n");

    if (uc) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "use_count: ";
        m_log.out << m_log.chl << uc;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "this: ";
    m_log.out << m_log.chl << &*this;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "position: ";
    m_log.out << m_log.chl << m_position;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    // recursive begin
    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "production: ";
        m_production->json(sc + 2, true, 0, true, m_production.use_count());

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "lookahead_symbols: [";
        m_log.out << (m_lookahead_symbols.size() ? "\n" : "");
        for (auto l : m_lookahead_symbols)
            l->json(sc + 4, true, sc + 4, true, l.use_count());
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "]\n";
    }
    // recursive end

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
