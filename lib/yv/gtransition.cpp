#include "include/gtransition.hpp"
#include "include/gstate.hpp"

#include <cassert>
#include <sstream>

// TODO INVALID_INDEX -> -1
gtransition::gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state)
    : m_symbol(symbol),
      m_state(state),
      m_index(INVALID_INDEX),
      m_log("yyv", "gtrans", 0) {}

int gtransition::index() const {
    return m_index;
}

const std::shared_ptr<gsymbol> &gtransition::symbol() const {
    return m_symbol;
}

const std::shared_ptr<gstate> &gtransition::state() const {
    return m_state;
}

bool gtransition::operator<(const gtransition &transition) const {
    assert(m_symbol->index() >= 0);
    assert(transition.m_symbol->index() >= 0);
    return m_symbol->index() < transition.m_symbol->index();
}

std::string gtransition::microdump() const {
    std::stringstream s;
    s << &*this << " ";
    s << m_index << " ";
    s << m_symbol->microdump() << " ";
    s << m_state->microdump() << "";
    return s.str();
}

void gtransition::json(int sc, bool nested, int in) const {
    m_log.out << m_log.chl << m_log.sp(in) << "gtrans: " << m_log.cnr << "{\n";

    m_log.out << m_log.cnr << m_log.sp(sc + 2) << "this: ";
    m_log.out << m_log.chl << &*this << ",\n";

    m_log.out << m_log.cnr << m_log.sp(sc + 2) << "index: ";
    m_log.out << m_log.chl << m_index << ",\n";

    // recursive begin
    if (nested) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "state: ";
        m_log.out << m_log.cwhite << "<" << m_state.use_count() << "> ";
        m_state->json(sc + 2, true, 0);
    }

    // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "lookahead_symbols: [\n";
    // for (auto l : m_lookahead_symbols)
    //     l.json(sc + 4);
    // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "]\n";

    // recursive end

    m_log.out << m_log.cnr << m_log.sp(sc) << "},\n";
}
