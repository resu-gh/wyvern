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
    /*debug*/ m_log.set_fun("count_refs");
    int references = 0;

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << "(produc).m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        /*debug*/ m_log.trace(1) << m_log.op("symbol");
        /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
        /*debug*/ m_log.out << i->get()->microdump() << "\n";

        references += (&*symbol.get() == &*i->get() ? 1 : 0);
        // clang-format off
        /*debug*/ if (&*symbol.get() == &*i->get())
        /*debug*/     m_log.trace(1) << m_log.op("incr") << "references: " << m_log.cwhite << references << "\n";
        // clang-format on
    }

    references += (&*m_precedence_symbol.get() == &*symbol.get() ? 1 : 0);
    // clang-format off
    /*debug*/ if (&*m_precedence_symbol.get() == &*symbol.get())
    /*debug*/     m_log.trace(1) << m_log.op("incr") << "references: " << m_log.cwhite << references << "\n";
    // clang-format on

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

    while (i!= m_symbols.rend() && i->get()->symbol_type() != gsymboltype::SYMBOL_TERMINAL)
        ++i;

    return i != m_symbols.rend() ? *i : nullptr;
}

std::string gproduction::microdump() const {
    std::stringstream s;
    s << (void *)&*this << " ";
    s << m_index << " ";
    return s.str();
}
