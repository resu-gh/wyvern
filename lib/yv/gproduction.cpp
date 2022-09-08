#include "include/gproduction.hpp"
#include <cassert>

gproduction::gproduction(int index,
                         const std::shared_ptr<gsymbol> &symbol,
                         int line,
                         int column,
                         const std::shared_ptr<gaction> &action)
    : m_index(index),
      m_symbol(symbol),
      m_line(line),
      m_column(column),
      m_action(action),
      m_precedence_symbol(nullptr) {
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

void gproduction::append_symbol(const std::shared_ptr<gsymbol> &symbol) {
    m_symbols.push_back(symbol);
}
