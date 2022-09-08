#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"
#include "include/gsymbol.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

ggrammar::ggrammar()
    : m_identifier(),
      m_symbols(),
      m_productions(),
      m_active_whitespace_directive(false),
      m_active_precedence_directive(false),
      m_associativity(gsymbolassoc::ASSOCIATE_NULL),
      m_precedence(0),
      m_active_production(nullptr),
      m_active_symbol(nullptr),
      m_start_symbol(nullptr),
      m_end_symbol(nullptr),
      m_error_symbol(nullptr) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "(constructor) " << m_log.cwhite << "\n";
    m_start_symbol = add_symbol(".start", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
    /*debug*/ m_log.out << m_log.cggram << "             check  START ";
    /*debug*/ m_log.out << m_log.cwhite << (void *)&*m_start_symbol << " ";
    /*debug*/ m_log.out << "[uc: " << m_start_symbol.use_count() << "] ";
    /*debug*/ m_log.out << m_start_symbol->index() << " ";
    /*debug*/ m_log.out << m_start_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cggram << "(constructor)";
    /*debug*/ m_log.out << m_log.creset << "\n";
    m_end_symbol = add_symbol(".end", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_END);
    /*debug*/ m_log.out << m_log.cggram << "             check    END ";
    /*debug*/ m_log.out << m_log.cwhite << (void *)&*m_end_symbol << " ";
    /*debug*/ m_log.out << "[uc: " << m_end_symbol.use_count() << "] ";
    /*debug*/ m_log.out << m_end_symbol->index() << " ";
    /*debug*/ m_log.out << m_end_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cggram << "(constructor)";
    /*debug*/ m_log.out << m_log.creset << "\n";
    m_error_symbol = add_symbol("error", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NULL);
    /*debug*/ m_log.out << m_log.cggram << "             check  ERROR ";
    /*debug*/ m_log.out << m_log.cwhite << (void *)&*m_error_symbol << " ";
    /*debug*/ m_log.out << "[uc: " << m_error_symbol.use_count() << "] ";
    /*debug*/ m_log.out << m_error_symbol->index() << " ";
    /*debug*/ m_log.out << m_error_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cggram << "(constructor)";
    /*debug*/ m_log.out << m_log.creset << "\n";
}

const std::string &ggrammar::identifier() const {
    return m_identifier;
}

std::vector<std::shared_ptr<gsymbol>> &ggrammar::symbols() {
    return m_symbols;
}

const std::shared_ptr<gsymbol> &ggrammar::error_symbol() const {
    return m_error_symbol;
}

ggrammar &ggrammar::grammar(const std::string &identifier) {
    m_identifier = identifier;
    return *this;
}

ggrammar &ggrammar::left(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "proces   LEFT " << m_log.cwhite << "%left ";
    /*debug*/ m_log.out << m_log.cggram << "(left)\n";
    /*debug*/ m_log.out << m_log.cggram << "               set  ASSOC ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_LEFT;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set PRECED ";
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cmagenta << m_precedence << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DWHITE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set APRODU ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << " -> ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << " -> ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::right(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "proces  RIGHT " << m_log.cwhite << "%right\n";
    /*debug*/ m_log.out << m_log.cggram << "               set  ASSOC ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_RIGHT;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set PRECED ";
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cmagenta << m_precedence << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DWHITE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set APRODU ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << " -> ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << " -> ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::none(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "proces   NONE " << m_log.cwhite << "%none\n";
    /*debug*/ m_log.out << m_log.cggram << "               set  ASSOC ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NONE;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set PRECED ";
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cmagenta << m_precedence << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DWHITE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set APRODU ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << " -> ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << " -> ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::whitespace(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "proces WHITES " << m_log.cwhite << "%whitespace\n";
    /*debug*/ m_log.out << m_log.cggram << "               set  ASSOC ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DWHITE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = true;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set APRODU ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << " -> ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << " -> ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::precedence(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "proces PRECED " << m_log.cwhite << "%precedence\n";
    assert(m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "               get ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << "\n";
    if (m_active_symbol) {
        /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
        /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
        m_active_precedence_directive = true;
        /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false");
    }
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::production(const std::string &identifier, int line) {
    assert(!identifier.empty());
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "  init PRODUC " << m_log.cwhite << identifier;
    /*debug*/ m_log.out << m_log.cggram << " (production)\n";
    /*debug*/ m_log.out << m_log.cggram << "               set  ASSOC ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DWHITE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set APRODU ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << " -> ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << " -> " << m_log.cyellow << "ASYMBO\n";
    m_active_symbol = non_terminal_symbol(identifier, line);
    /*debug*/ m_log.out << m_log.cyellow << "               ... ASYMBO ";
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::end_production() {
    assert(m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "   end PRODUC " << m_log.cwhite << m_active_symbol->lexeme();
    /*debug*/ m_log.out << m_log.cggram << " (production)\n";
    /*debug*/ m_log.out << m_log.cggram << "               set  ASSOC ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DWHITE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set DPRECE ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set APRODU ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << " -> ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production;
    /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "] " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "               set ASYMBO ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << " -> ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol;
    /*debug*/ m_log.out << (m_active_symbol ? " [uc: " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "] " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

// TODO comments
ggrammar &ggrammar::error(int line) {
    assert(line >= 1);
    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        const std::shared_ptr<gsymbol> &symbol = error_symbol();
        symbol->set_associativity(m_associativity);
        symbol->set_precedence(m_precedence);
    } else if (m_active_symbol) {
        if (!m_active_production)
            m_active_production = add_production(m_active_symbol, line);
        m_active_production->append_symbol(error_symbol());
    }
    return *this;
}

const std::shared_ptr<gsymbol> &ggrammar::non_terminal_symbol(const std::string &lexeme, int line) {
    assert(!lexeme.empty());
    assert(line >= 0);
    return add_symbol(lexeme, line, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
}

const std::shared_ptr<gsymbol> &ggrammar::add_symbol(const std::string &lexeme, int line, glexemetype lexeme_type, gsymboltype symbol_type) {
    assert(!lexeme.empty());
    assert(line >= 0);
    std::vector<std::shared_ptr<gsymbol>>::const_iterator i = m_symbols.begin();
    while (i != m_symbols.end() && !i->get()->matches(lexeme, symbol_type))
        ++i;
    if (i == m_symbols.end()) {
        std::shared_ptr<gsymbol> symbol = std::make_shared<gsymbol>(gsymbol(lexeme));
        symbol->set_line(line);
        symbol->set_lexeme_type(lexeme_type);
        symbol->set_symbol_type(symbol_type);
        /*debug*/ m_log.out << m_log.cggram << "            ";
        /*debug*/ m_log.out << "create SYMBOL " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*symbol << " ";
        /*debug*/ m_log.out << "[uc: " << symbol.use_count() << "] ";
        /*debug*/ m_log.out << symbol->index() << " ";
        /*debug*/ m_log.out << symbol->lexeme() << " ";
        /*debug*/ m_log.out << m_log.cggram << "(add_symbol)";
        /*debug*/ m_log.out << m_log.creset << "\n";
        m_symbols.push_back(std::move(symbol));
        /*debug*/ m_log.out << m_log.cggram << "            ";
        /*debug*/ m_log.out << "<-move SYMBOL " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_symbols.back() << " ";
        /*debug*/ m_log.out << "[uc: " << m_symbols.back().use_count() << "] ";
        /*debug*/ m_log.out << m_symbols.back()->index() << " ";
        /*debug*/ m_log.out << m_symbols.back()->lexeme();
        /*debug*/ m_log.out << m_log.creset << "\n";
        return m_symbols.back();
    }
    assert(i->get());
    assert(i->get()->symbol_type() == symbol_type);
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << "found  SYMBOL " << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*i << " ";
    /*debug*/ m_log.out << i->get()->lexeme() << " ";
    /*debug*/ m_log.out << "[uc: " << i->use_count() << "]";
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *i;
}

const std::shared_ptr<gproduction> &ggrammar::add_production(const std::shared_ptr<gsymbol> &symbol, int line) {
    assert(symbol);
    assert(line > 0);
    if (m_productions.empty()) {
        assert(m_start_symbol);
        std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), m_start_symbol, 0, 0, nullptr);
        /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
        /*debug*/ m_log.out << "create PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << " ";
        /*debug*/ m_log.out << "[uc: " << production.use_count() << "] ";
        /*debug*/ m_log.out << m_log.cggram << "(add_production)\n";
        /*debug*/ m_log.out << "                on SYMBOL ";
        /*debug*/ m_log.out << m_log.creset;
        /*debug*/ m_log.out << &*production.get()->symbol() << " ";
        /*debug*/ m_log.out << "[uc: " << production.get()->symbol().use_count() << "] ";
        /*debug*/ m_log.out << production.get()->symbol()->index() << " ";
        /*debug*/ m_log.out << production.get()->symbol()->lexeme() << "\n";
        /*debug*/ m_log.out << m_log.cggram << "              with ACTION ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << production.get()->action() << " ";
        /*debug*/ m_log.out << (production.get()->action() ? "[uc: " : "");
        /*debug*/ m_log.out << (production.get()->action() ? std::to_string(production.get()->action().use_count()) : "");
        /*debug*/ m_log.out << (production.get()->action() ? "] " : "");
        /*debug*/ m_log.out << (production.get()->action() ? std::to_string(production.get()->action()->index()) : "");
        /*debug*/ m_log.out << (production.get()->action() ? " " : "");
        /*debug*/ m_log.out << (production.get()->action() ? production.get()->action()->identifier() : "");
        /*debug*/ m_log.out << m_log.cggram << "\n                      ---";
        /*debug*/ m_log.out << m_log.creset << "\n";
        production->append_symbol(symbol);
        /*debug*/ m_log.out << m_log.cggram << "            ";
        /*debug*/ m_log.out << "pushed SYMBOL " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*symbol << " ";
        /*debug*/ m_log.out << "[uc: " << symbol.use_count() << "] ";
        /*debug*/ m_log.out << symbol->index() << " ";
        /*debug*/ m_log.out << symbol->lexeme() << " ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        /*debug*/ m_log.out << "              to-> PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << " ";
        /*debug*/ m_log.out << "[uc: " << production.use_count() << "]\n" << m_log.cggram;
        /*debug*/ m_log.out << "             check SYMBOL " << m_log.cwhite;
        /*debug*/ m_log.out << &*production->symbols().back() << " ";
        /*debug*/ m_log.out << "[uc: " << production->symbols().back().use_count() << "] ";
        /*debug*/ m_log.out << production->symbols().back()->index() << " ";
        /*debug*/ m_log.out << production->symbols().back()->lexeme() << " ";
        /*debug*/ m_log.out << m_log.cggram << "\n                      ---";
        /*debug*/ m_log.out << m_log.creset << "\n";
        m_start_symbol->append_production(production);
        /*debug*/ m_log.out << m_log.cggram << "            ";
        /*debug*/ m_log.out << "pushed PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << " ";
        /*debug*/ m_log.out << "[uc: " << production.use_count() << "] ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        /*debug*/ m_log.out << "              to->  START " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_start_symbol << " ";
        /*debug*/ m_log.out << "[uc: " << m_start_symbol.use_count() << "] ";
        /*debug*/ m_log.out << m_start_symbol->index() << " ";
        /*debug*/ m_log.out << m_start_symbol->lexeme() << " ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        /*debug*/ m_log.out << "             check  START " << m_log.cwhite;
        /*debug*/ m_log.out << &*m_start_symbol->productions().back() << " ";
        /*debug*/ m_log.out << "[uc: " << m_start_symbol->productions().back().use_count() << "] ";
        /*debug*/ m_log.out << m_log.cggram << "\n                      ---";
        /*debug*/ m_log.out << m_log.creset << "\n";
        /*debug*/ m_log.out << m_log.cggram << "            ";
        /*debug*/ m_log.out << "<-move PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << " ";
        /*debug*/ m_log.out << "[uc: " << production.use_count() << "] ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        m_productions.push_back(std::move(production));
        /*debug*/ m_log.out << m_log.cggram << "            ";
        /*debug*/ m_log.out << " check PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_productions.back() << " ";
        /*debug*/ m_log.out << "[uc: " << m_productions.back().use_count() << "] ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        return m_productions.back();
    }
    std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), symbol, line, -1, nullptr);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram = ";
    /*debug*/ m_log.out << "create PRODUC " << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*production << " ";
    /*debug*/ m_log.out << "[uc: " << production.use_count() << "] ";
    /*debug*/ m_log.out << m_log.cggram << "(add_production)\n";
    /*debug*/ m_log.out << "                on SYMBOL ";
    /*debug*/ m_log.out << m_log.creset;
    /*debug*/ m_log.out << &*production.get()->symbol() << " ";
    /*debug*/ m_log.out << "[uc: " << production.get()->symbol().use_count() << "] ";
    /*debug*/ m_log.out << production.get()->symbol()->index() << " ";
    /*debug*/ m_log.out << production.get()->symbol()->lexeme() << "\n";
    /*debug*/ m_log.out << m_log.cggram << "              with ACTION ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << production.get()->action() << " ";
    /*debug*/ m_log.out << (production.get()->action() ? "[uc: " : "");
    /*debug*/ m_log.out << (production.get()->action() ? std::to_string(production.get()->action().use_count()) : "");
    /*debug*/ m_log.out << (production.get()->action() ? "] " : "");
    /*debug*/ m_log.out << (production.get()->action() ? std::to_string(production.get()->action()->index()) : "");
    /*debug*/ m_log.out << (production.get()->action() ? " " : "");
    /*debug*/ m_log.out << (production.get()->action() ? production.get()->action()->identifier() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n                      ---";
    /*debug*/ m_log.out << m_log.creset << "\n";
    symbol->append_production(production);
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << "pushed PRODUC " << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*production << " ";
    /*debug*/ m_log.out << "[uc: " << production.use_count() << "] ";
    /*debug*/ m_log.out << m_log.cggram << "\n";
    /*debug*/ m_log.out << "              to-> SYMBOL " << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*symbol << " ";
    /*debug*/ m_log.out << "[uc: " << symbol.use_count() << "] ";
    /*debug*/ m_log.out << symbol->index() << " ";
    /*debug*/ m_log.out << symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cggram << "\n";
    /*debug*/ m_log.out << "             check  START " << m_log.cwhite;
    /*debug*/ m_log.out << &*symbol->productions().back() << " ";
    /*debug*/ m_log.out << "[uc: " << symbol->productions().back().use_count() << "] ";
    /*debug*/ m_log.out << m_log.cggram << "\n                      ---";
    /*debug*/ m_log.out << m_log.creset << "\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << "<-move PRODUC " << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*production << " ";
    /*debug*/ m_log.out << "[uc: " << production.use_count() << "] ";
    /*debug*/ m_log.out << m_log.cggram << "\n";
    m_productions.push_back(std::move(production));
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << " check PRODUC " << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_productions.back() << " ";
    /*debug*/ m_log.out << "[uc: " << m_productions.back().use_count() << "] ";
    /*debug*/ m_log.out << m_log.cggram << "\n";
    return m_productions.back();
}
