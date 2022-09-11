#include "include/gproduction.hpp"
#include <cassert>
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

std::string gproduction::microdump() const {
    std::stringstream s;
    s << (void *)&*this << " ";
    s << m_index << " ";
    return s.str();
}
