#include "include/gstate.hpp"
#include "include/gitem.hpp"
#include "include/gtransition.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

// TODO INVALID_INDEX -> -1
gstate::gstate()
    : m_index(INVALID_INDEX),
      m_processed(false),
      m_items(),
      m_transitions(),
      m_log("yvv", "gstat", 0) {}

int gstate::index() const {
    return m_index;
}

bool gstate::processed() const {
    return m_processed;
}

const std::set<gitem> &gstate::items() const {
    return m_items;
}

const std::set<gtransition> &gstate::transitions() const {
    return m_transitions;
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

// TODO custom impl BEGIN
std::set<gtransition>::iterator gstate::find_transition_by_symbol(const std::shared_ptr<gsymbol> &symbol) {
    if (symbol.get()) {
        std::set<gtransition>::iterator i = m_transitions.begin();

        while (i != m_transitions.end() && !i->taken_on_symbol(symbol))
            ++i;

        if (i != m_transitions.end())
            return i;
    }

    return m_transitions.end();
}
// TODO custom impl END

bool gstate::operator<(const gstate &state) const {
    return std::lexicographical_compare(m_items.begin(), m_items.end(), state.m_items.begin(), state.m_items.end());
}

std::string gstate::microdump() const {
    std::stringstream s;
    s << &*this << " ";
    s << m_index << " ";
    s << (m_processed ? "true" : "false") << " ";
    s << "items[" << m_items.size() << "] ";
    s << "trans[" << m_transitions.size() << "] ";
    return s.str();
}

void gstate::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "gstate";
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

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "processed: ";
    m_log.out << m_log.chl << (m_processed ? "true" : "false");
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    // recursive begin
    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "items: [";
        m_log.out << (m_items.size() ? "\n" : "");
        for (auto i : m_items)
            i.json(sc + 4, true, sc + 4, true);
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";
    }

    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "transitions: [";
        m_log.out << (m_transitions.size() ? "\n" : "");
        for (auto t : m_transitions)
            t.json(sc + 4, true, sc + 4, true);
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";
    }
    // recursive end

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
