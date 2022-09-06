#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"
#include <cassert>
#include <iostream>

ggrammar::ggrammar()
    : m_identifier(),
      m_active_whitespace_directive(false),
      m_active_precedence_directive(false),
      m_associativity(gsymbolassoc::ASSOCIATE_NULL),
      m_precedence(0),
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

ggrammar &ggrammar::left(int line) {
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "MATCH ";
    m_log.out << m_log.cyellow << "%left ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    m_associativity = gsymbolassoc::ASSOCIATE_LEFT;
    ++m_precedence;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

ggrammar &ggrammar::right(int line) {
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "MATCH ";
    m_log.out << m_log.cyellow << "%right ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    m_associativity = gsymbolassoc::ASSOCIATE_RIGHT;
    ++m_precedence;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

ggrammar &ggrammar::none(int line) {
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "MATCH ";
    m_log.out << m_log.cyellow << "%none ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    m_associativity = gsymbolassoc::ASSOCIATE_NONE;
    ++m_precedence;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

ggrammar &ggrammar::whitespace() {
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "MATCH ";
    m_log.out << m_log.cyellow << "%whitespace\n";
    m_log.out << m_log.creset;
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    m_active_whitespace_directive = true;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr; // TODO
    return *this;
}

ggrammar &ggrammar::precedence() {
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "MATCH ";
    m_log.out << m_log.cyellow << "%precedence\n";
    m_log.out << m_log.creset;
    assert(m_active_symbol);
    if (m_active_symbol)
        m_active_precedence_directive = true;
    return *this;
}