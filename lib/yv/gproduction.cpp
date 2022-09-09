#include "include/gproduction.hpp"
#include <cassert>

gproduction::gproduction(int index, const std::shared_ptr<gsymbol> &symbol, int line, int column, const std::shared_ptr<gaction> &action)
    : m_index(index),
      m_symbol(symbol),
      m_line(line),
      m_column(column),
      m_action(action),
      m_precedence_symbol(nullptr),
      m_log() {
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

void gproduction::dump(int separator, const std::string &color) {
    std::string sep(separator, ' ');
    m_log.out << sep << color << "&*this ";
    m_log.out << m_log.cwhite << (void *)&*this << "\n";
    m_log.out << sep << color << ".index ";
    m_log.out << m_log.cwhite << m_index << "\n";
}
