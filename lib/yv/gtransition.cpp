#include "include/gtransition.hpp"
#include "include/gstate.hpp"
#include "include/gtranstype.hpp"

#include <cassert>
#include <sstream>

// TODO INVALID_INDEX -> -1
gtransition::gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gstate> &state)
    : m_symbol(symbol),
      m_state(state),
      m_reduced_symbol(),
      m_index(gtransition::INVALID_INDEX),
      m_precedence(0),
      m_action(gaction::INVALID_INDEX),
      m_reduced_length(0),
      m_type(gtranstype::TRANSITION_SHIFT),
      m_log("yyv", "gtrans", 0) {
    assert(m_symbol.get());
    assert(m_state.get());
}

gtransition::gtransition(const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gsymbol> &reduced_symbol, int reduced_length, int precedence, int action)
    : m_symbol(symbol),
      m_state(),
      m_reduced_symbol(reduced_symbol),
      m_index(gtransition::INVALID_INDEX),
      m_precedence(precedence),
      m_action(action),
      m_reduced_length(reduced_length),
      m_type(gtranstype::TRANSITION_REDUCE),
      m_log("yyv", "gtrans", 0) {
    assert(m_reduced_symbol.get());
    assert(m_reduced_length >= 0);
    assert(m_precedence >= 0);
}

int gtransition::index() const {
    return m_index;
}

int gtransition::precedence() const {
    return m_precedence;
}

int gtransition::action() const {
    return m_action;
}

int gtransition::reduced_length() const {
    return m_reduced_length;
}

gtranstype gtransition::type() const {
    return m_type;
}

const std::shared_ptr<gsymbol> &gtransition::symbol() const {
    return m_symbol;
}

const std::shared_ptr<gsymbol> &gtransition::reduced_symbol() const {
    return m_reduced_symbol;
}

const std::shared_ptr<gstate> &gtransition::state() const {
    return m_state;
}

void gtransition::set_index(int index) const {
    m_index = index;
}

bool gtransition::taken_on_symbol(const std::shared_ptr<gsymbol> &symbol) const {
    return m_symbol == symbol;
}

void gtransition::override_shift_to_reduce(const std::shared_ptr<gsymbol> &symbol, int length, int precedence, int action) const {
    assert(m_type == gtranstype::TRANSITION_SHIFT);
    assert(m_state.get());
    assert(!m_reduced_symbol.get());
    assert(length >= 0);
    assert(precedence >= 0);
    assert(symbol.get());

    m_type = gtranstype::TRANSITION_REDUCE;
    m_state = nullptr;
    m_reduced_symbol = symbol;
    m_precedence = precedence;
    m_action = action;
}

void gtransition::override_reduce_to_reduce(const std::shared_ptr<gsymbol> &symbol, int length, int precedence, int action) const {
    assert(m_type == gtranstype::TRANSITION_REDUCE);
    assert(m_reduced_symbol.get());
    assert(symbol.get());
    assert(length >= 0);
    assert(precedence >= 0);
    assert(m_reduced_symbol != symbol); // TODO FIXME pointers comparison

    m_reduced_symbol = symbol;
    m_reduced_length = length;
    m_precedence = precedence;
    m_action = action;
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

void gtransition::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "gtrans";
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

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "type: ";
    m_log.out << m_log.chl << m_type;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    if (!inlined) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "precedence: ";
        m_log.out << m_log.chl << m_precedence;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "action_index: ";
        m_log.out << m_log.chl << m_action;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "reduced_length: ";
        m_log.out << m_log.chl << m_reduced_length;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    // recursive begin
    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "state: ";
        if (m_state)
            m_state->json(sc + 2, true, 0, true, m_state.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "symbol: ";
        if (m_symbol)
            m_symbol->json(sc + 2, true, 0, true, m_symbol.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "reduced_symbol: ";
        if (m_reduced_symbol)
            m_reduced_symbol->json(sc + 2, true, 0, true, m_reduced_symbol.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";
    }
    // recursive end

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
