#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"

ggrammar::ggrammar()
    : m_identifier(),
      m_active_whitespace_directive(false),
      m_active_precedence_directive(false),
      m_associativity(gsymbolassoc::ASSOCIATE_NULL),
      m_active_production(nullptr),
      m_active_symbol(nullptr) {}

const std::string &ggrammar::identifier() const {
    return m_identifier;
}

ggrammar &ggrammar::grammar(const std::string &identifier) {
    m_identifier = identifier;
    // maybe unused return
    return *this;
}

ggrammar &ggrammar::whitespace() {
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    m_active_whitespace_directive = true;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr; // TODO
    return *this;
}
