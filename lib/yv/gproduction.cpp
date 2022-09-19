#include "include/gproduction.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

gproduction::gproduction(int index, const std::shared_ptr<gsymbol> &symbol, int line, int column, const std::shared_ptr<gaction> &action)
    : m_index(index),
      m_symbol(symbol),
      m_line(line),
      m_column(column),
      m_action(action),
      m_symbols(),
      m_precedence_symbol(nullptr),
      m_log("yyv", "gprod", 255) {
    assert(m_symbol);
}

int gproduction::index() const {
    return m_index;
}

const std::shared_ptr<gsymbol> &gproduction::symbol() const {
    return m_symbol;
}

int gproduction::line() const {
    return m_line;
}

int gproduction::column() const {
    return m_column;
}

const std::shared_ptr<gaction> &gproduction::action() const {
    return m_action;
}

const std::vector<std::shared_ptr<gsymbol>> &gproduction::symbols() const {
    return m_symbols;
}

const std::shared_ptr<gsymbol> &gproduction::precedence_symbol() const {
    return m_precedence_symbol;
}

void gproduction::set_precedence_symbol(const std::shared_ptr<gsymbol> &symbol) {
    assert(symbol);
    assert(!m_precedence_symbol);
    m_precedence_symbol = symbol;
}

void gproduction::set_action(const std::shared_ptr<gaction> &action) {
    m_action = action;
}

void gproduction::append_symbol(const std::shared_ptr<gsymbol> &symbol) {
    m_symbols.push_back(symbol);
}

int gproduction::count_references_to_symbol(const std::shared_ptr<gsymbol> &symbol) {
    int references = 0;

    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i)
        references += (&*symbol.get() == &*i->get() ? 1 : 0);

    references += (&*m_precedence_symbol.get() == &*symbol.get() ? 1 : 0);

    return references;
}

int gproduction::length() const {
    return int(m_symbols.size());
}

// TODO FIXME?
void gproduction::replace_references_to_symbol(const std::shared_ptr<gsymbol> &to_symbol, const std::shared_ptr<gsymbol> &with_symbol) {
    if (m_symbol == to_symbol)
        m_symbol = with_symbol;
    if (m_precedence_symbol == to_symbol)
        m_precedence_symbol = with_symbol;
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        if (i->get() == to_symbol.get())
            *i = with_symbol;
    }
}

int gproduction::precedence() const {
    return m_precedence_symbol ? m_precedence_symbol->precedence() : 0;
}

std::shared_ptr<gsymbol> gproduction::find_rightmost_terminal_symbol() const {
    using symb_riter = std::vector<std::shared_ptr<gsymbol>>::const_reverse_iterator;
    symb_riter i = m_symbols.rbegin();

    while (i != m_symbols.rend() && i->get()->symbol_type() != gsymboltype::SYMBOL_TERMINAL)
        ++i;

    return i != m_symbols.rend() ? *i : nullptr;
}

std::shared_ptr<gsymbol> gproduction::symbol_by_position(int position) const {
    return (position >= 0 && position < int(m_symbols.size())) ? m_symbols[position] : nullptr;
}

int gproduction::action_index() const {
    return m_action ? m_action->index() : gaction::INVALID_INDEX; // -1
}

std::string gproduction::microdump() const {
    std::stringstream s;
    s << (void *)&*this << " ";
    s << m_index << " ";
    return s.str();
}

void gproduction::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "gprod";
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

    if (!inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "line" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_line << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "column" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_column << m_log.cnr << ",\n";
    }

    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "symbol" << m_log.cnr << ": ";
        m_symbol->json(sc + 2, true, 0, true, m_symbol.use_count());

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "action" << m_log.cnr << ": ";
        if (m_action)
            m_action->json(sc + 2, true, 0, true, m_action.use_count());
        else
            m_log.out << m_log.chl << "nullptr\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "symbols: [";
        m_log.out << (m_symbols.size() ? "\n" : "");
        for (auto s : m_symbols)
            s->json(sc + 4, true, sc + 4, true, s.use_count());
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "precedence_symbol" << m_log.cnr << ": ";
        if (m_precedence_symbol)
            m_precedence_symbol->json(sc + 2, true, 0, true, m_precedence_symbol.use_count());
        else
            m_log.out << m_log.chl << "nullptr\n";
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
