#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"
#include "include/gsymbol.hpp"
#include "include/xtoken.hpp"
#include "include/xtokentype.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

ggrammar::ggrammar()
    : m_identifier(),
      m_symbols(),
      m_productions(),
      m_actions(),
      m_whitespace_tokens(),
      m_active_whitespace_directive(false),
      m_active_precedence_directive(false),
      m_associativity(gsymbolassoc::ASSOCIATE_NULL),
      m_precedence(0),
      m_active_production(nullptr),
      m_active_symbol(nullptr),
      m_start_symbol(nullptr),
      m_end_symbol(nullptr),
      m_error_symbol(nullptr) {
    /*debug*/ m_log.out << m_log.cggram;
    /*debug*/ m_log.out << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   constructor()\n";
    m_start_symbol = add_symbol(".start", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
    assert(m_start_symbol->lexeme() == ".start");
    assert(m_start_symbol->line() == 0);
    assert(m_start_symbol->lexeme_type() == glexemetype::LEXEME_NULL);
    assert(m_start_symbol->symbol_type() == gsymboltype::SYMBOL_NON_TERMINAL);
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   constructor()\n";
    /*debug*/ m_log.out << m_log.cggram;
    /*debug*/ m_log.out << "   check   .m_start_symbol ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_start_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_start_symbol) << ") ";
    /*debug*/ m_log.out << "<" << m_start_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_start_symbol->index() << " ";
    /*debug*/ m_log.out << m_start_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.creset << "\n";
    m_end_symbol = add_symbol(".end", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_END);
    assert(m_end_symbol->lexeme() == ".end");
    assert(m_end_symbol->line() == 0);
    assert(m_end_symbol->lexeme_type() == glexemetype::LEXEME_NULL);
    assert(m_end_symbol->symbol_type() == gsymboltype::SYMBOL_END);
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   constructor()\n";
    /*debug*/ m_log.out << m_log.cggram;
    /*debug*/ m_log.out << "   check   .m_end_symbol   ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_end_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_end_symbol) << ") ";
    /*debug*/ m_log.out << "<" << m_end_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_end_symbol->index() << " ";
    /*debug*/ m_log.out << m_end_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.creset << "\n";
    m_error_symbol = add_symbol("error", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NULL);
    assert(m_error_symbol->lexeme() == "error");
    assert(m_error_symbol->line() == 0);
    assert(m_error_symbol->lexeme_type() == glexemetype::LEXEME_NULL);
    assert(m_error_symbol->symbol_type() == gsymboltype::SYMBOL_NULL);
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   constructor()\n";
    /*debug*/ m_log.out << m_log.cggram;
    /*debug*/ m_log.out << "   check   .m_error_symbol ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_error_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_error_symbol) << ") ";
    /*debug*/ m_log.out << "<" << m_error_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_error_symbol->index() << " ";
    /*debug*/ m_log.out << m_error_symbol->lexeme() << " ";
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
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   left()\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_associativity ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_LEFT;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_precedence ";
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cmagenta << m_precedence << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_production\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "    set    .m_active_symbol\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::right(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   right()\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_associativity ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_RIGHT;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_precedence ";
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cmagenta << m_precedence << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_production\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "    set    .m_active_symbol\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::none(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   none()\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_associativity ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NONE;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_precedence ";
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cmagenta << m_precedence << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_production\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "    set    .m_active_symbol\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::whitespace(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   whitespace()\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_associativity ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = true;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_production\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "    set    .m_active_symbol\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::precedence(int line) {
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   precedence()\n";
    assert(m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "     get   .m_active_symbol ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    if (m_active_symbol) {
        /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
        /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
        m_active_precedence_directive = true;
        /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    }
    /*debug*/ m_log.out << m_log.creset;
    return *this;
}

ggrammar &ggrammar::production(const std::string &identifier, int line) {
    assert(!identifier.empty());
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   production()\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_associativity ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_production\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "    set    .m_active_symbol\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    /*debug*/ m_log.out << m_log.cyellow << "REF\n";
    m_active_symbol = non_terminal_symbol(identifier, line);
    /*debug*/ m_log.out << m_log.cmagenta << "    call   production()\n";
    /*debug*/ m_log.out << m_log.cyellow << "     REF   ";
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

ggrammar &ggrammar::end_production() {
    assert(m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   end_production()\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_associativity ";
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cmagenta << m_associativity << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_whitespace_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cmagenta << (m_active_precedence_directive ? "true" : "false") << "\n";
    /*debug*/ m_log.out << m_log.cggram << "     set   .m_active_production\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_production << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
    /*debug*/ m_log.out << (m_active_production ? "<" : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
    /*debug*/ m_log.out << (m_active_production ? "> " : "");
    /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.out << m_log.cggram << "    set    .m_active_symbol\n";
    /*debug*/ m_log.out << m_log.cggram << "            ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.cggram << "\n            ";
    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << (void *)&*m_active_symbol << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_symbol) << ") ";
    /*debug*/ m_log.out << (m_active_symbol ? "<" : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol.use_count()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? "> " : "");
    /*debug*/ m_log.out << (m_active_symbol ? std::to_string(m_active_symbol->index()) : "");
    /*debug*/ m_log.out << (m_active_symbol ? " " : "");
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->lexeme() : "");
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *this;
}

// TODO comments
ggrammar &ggrammar::error(int line) {
    assert(line >= 1);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   error()\n";
    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.out << m_log.cggram << "      if   ";
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity == ASSOCIATE_NULL (case 1)\n";
        const std::shared_ptr<gsymbol> &symbol = error_symbol();
        /*debug*/ m_log.out << m_log.cggram << "     get    error SYMBOL ";
        /*debug*/ m_log.out << m_log.cwhite << (void *)&*symbol << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*symbol) << ") ";
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->index() << " ";
        /*debug*/ m_log.out << symbol->lexeme() << "\n";
        /*debug*/ m_log.out << m_log.cggram << "     set    symbol .associativity ";
        /*debug*/ m_log.out << m_log.cwhite << symbol->associativity() << " -> ";
        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.cmagenta << symbol->associativity() << "\n";
        /*debug*/ m_log.out << m_log.cggram << "     set    symbol .precedence ";
        /*debug*/ m_log.out << m_log.cwhite << symbol->precedence() << " -> ";
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cmagenta << symbol->precedence() << "";
        /*debug*/ m_log.out << m_log.creset << "\n";
    } else if (m_active_symbol) {
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
        /*debug*/ m_log.out << m_log.cggram << "      if   ";
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol == nullptr (case 2)\n";
        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            /*debug*/ m_log.out << m_log.cggram << "      if   ";
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production != nullptr (case 3)\n";
            /*debug*/ m_log.out << m_log.cggram << "     get   .m_active_production ";
            /*debug*/ m_log.out << m_log.cwhite;
            /*debug*/ m_log.out << (void *)&*m_active_production << "(";
            /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production) << ") ";
            /*debug*/ m_log.out << (m_active_production ? "<" : "");
            /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
            /*debug*/ m_log.out << (m_active_production ? "> " : "");
            /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
            /*debug*/ m_log.out << " -> ";
            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.out << m_log.cyellow << "               ... APRODU ";
            /*debug*/ m_log.out << m_log.cwhite;
            /*debug*/ m_log.out << (void *)&*m_active_production;
            /*debug*/ m_log.out << (m_active_production ? " [uc: " : "");
            /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production.use_count()) : "");
            /*debug*/ m_log.out << (m_active_production ? "] " : "");
            /*debug*/ m_log.out << (m_active_production ? std::to_string(m_active_production->index()) : "");
            /*debug*/ m_log.out << m_log.cggram << " (error)\n";
        }
        m_active_production->append_symbol(error_symbol());
        /*debug*/ m_log.out << m_log.cggram << "    push    error SYMBOL ";
        /*debug*/ m_log.out << m_log.cwhite << &*error_symbol();
        /*debug*/ m_log.out << "(" << reinterpret_cast<std::uintptr_t>(&*error_symbol()) << ") ";
        /*debug*/ m_log.out << m_log.cwhite << "<" << error_symbol().use_count() << "> ";
        /*debug*/ m_log.out << m_log.cwhite << error_symbol()->index() << " ";
        /*debug*/ m_log.out << m_log.cwhite << error_symbol()->lexeme() << "\n";
        /*debug*/ m_log.out << m_log.cggram << "      to    .m_active_production.symbols ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_active_production->symbols().back() << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_active_production->symbols().back()) << ") <";
        /*debug*/ m_log.out << m_active_production->symbols().back().use_count() << "> ";
        /*debug*/ m_log.out << m_active_production->symbols().back()->index() << " ";
        /*debug*/ m_log.out << m_active_production->symbols().back()->lexeme() << " ";
        /*debug*/ m_log.out << m_log.creset << "\n";
    }
    return *this;
}

// TODO comment
ggrammar &ggrammar::literal(const std::string &literal, int line) {
    assert(!literal.empty());
    assert(line >= 0);
    assert(m_active_whitespace_directive || m_associativity != gsymbolassoc::ASSOCIATE_NULL || m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   literal()\n";
    if (m_active_whitespace_directive) {
        /*debug*/ m_log.out << m_log.cggram << "    if  ";
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_whitespace_directive == true (case 1)\n";
        /*debug*/ m_log.out << m_log.cerror << "TODO debug" << m_log.creset << "\n";
        m_whitespace_tokens.push_back(xtoken(xtokentype::TOKEN_LITERAL, 0, 0, nullptr, literal));
        /*debug*/ m_log.out << m_log.creset << "\n";
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
    } else if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.out << m_log.cggram << "      if   ";
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity != ASSOCIATE_NULL (case 2)\n";
        std::shared_ptr<gsymbol> symbol = literal_symbol(literal, line);
        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.cggram << "     set   new SYMBOL .associativity ";
        /*debug*/ m_log.out << m_log.cwhite << symbol->associativity() << "\n";
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cggram << "     set   new SYMBOL .precedence ";
        /*debug*/ m_log.out << m_log.cwhite << symbol->precedence() << "\n";
    } else if (m_active_symbol) {
        /*debug*/ m_log.out << m_log.cggram << "      if   ";
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 3)\n";
        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.cggram << "      if   ";
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 4)\n";
            /*debug*/ m_log.out << m_log.cerror << "TODO debug" << m_log.creset << "\n";
            m_active_production = add_production(m_active_symbol, line);
        }
        if (m_active_precedence_directive) {
            /*debug*/ m_log.out << m_log.cggram << "      if   ";
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true (case 5)\n";
            /*debug*/ m_log.out << m_log.cerror << "TODO debug" << m_log.creset << "\n";
            m_active_production->set_precedence_symbol(literal_symbol(literal, line));
            m_active_precedence_directive = false;
        } else {
            /*debug*/ m_log.out << m_log.cggram << "      if   ";
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false (case 6)\n";
            /*debug*/ m_log.out << m_log.cerror << "TODO debug" << m_log.creset << "\n";
            m_active_production->append_symbol(literal_symbol(literal, line));
        }
    }
    return *this;
}

ggrammar &ggrammar::regex(const std::string &regex, int line) {
    assert(!regex.empty());
    assert(line >= 0);
    assert(m_active_whitespace_directive || m_associativity != gsymbolassoc::ASSOCIATE_NULL || m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   regex()\n";
    if (m_active_whitespace_directive) {
        /*debug*/ m_log.out << m_log.cggram << "    if  ";
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_whitespace_directive == true (case 1)\n";
        m_whitespace_tokens.push_back(xtoken(xtokentype::TOKEN_REGULAR_EXPRESSION, 0, 0, nullptr, regex));
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
    } else if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        std::shared_ptr<gsymbol> symbol = regex_symbol(regex, line);
        symbol->set_associativity(m_associativity);
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
    } else if (m_active_symbol) {
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            m_active_production = add_production(m_active_symbol, line);
        }
        if (m_active_precedence_directive) {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            m_active_production->set_precedence_symbol(regex_symbol(regex, line));
            m_active_precedence_directive = false;
        } else {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            m_active_production->append_symbol(regex_symbol(regex, line));
        }
    }
    return *this;
}

ggrammar &ggrammar::identifier(const std::string &identifier, int line) {
    assert(!identifier.empty());
    assert(line >= 0);
    assert(m_active_symbol || m_associativity != gsymbolassoc::ASSOCIATE_NULL);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   identifier()\n";
    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
        std::shared_ptr<gsymbol> symbol = non_terminal_symbol(identifier, line);
        symbol->set_associativity(m_associativity);
        symbol->set_precedence(m_precedence);
    } else if (m_active_symbol) {
        /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            m_active_production = add_production(m_active_symbol, line);
        }
        if (m_active_precedence_directive) {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            m_active_production->set_precedence_symbol(non_terminal_symbol(identifier, line));
            m_active_precedence_directive = false;
        } else {
            /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
            m_active_production->append_symbol(non_terminal_symbol(identifier, line));
        }
    }
    return *this;
}

ggrammar &ggrammar::action(const std::string &identifier, int line) {
    assert(!identifier.empty());
    assert(line >= 1);
    assert(m_active_symbol);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   action()\n";
    if (m_active_symbol) {
        if (!m_active_production) {
            m_active_production = add_production(m_active_symbol, line);
        }
        m_active_production->set_action(add_action(identifier));
        m_active_production = nullptr;
    }
    return *this;
}

ggrammar &ggrammar::end_expression(int line) {
    assert(line >= 1);
    /*debug*/ m_log.out << m_log.cggram << "yv::ggram =\n";
    /*debug*/ m_log.out << m_log.cmagenta << "    call   end_expression()\n";
    // if active_symbol but not active_production
    // an empty production is being specified
    // the nil action marks the end of a production
    // for which no symbols have been specified
    if (m_active_symbol) {
        if (!m_active_production) {
            m_active_production = add_production(m_active_symbol, line);
        }
    }
    m_active_production = nullptr;
    return *this;
}

const std::shared_ptr<gsymbol> &ggrammar::non_terminal_symbol(const std::string &lexeme, int line) {
    assert(!lexeme.empty());
    assert(line >= 0);
    return add_symbol(lexeme, line, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
}

const std::shared_ptr<gsymbol> &ggrammar::literal_symbol(const std::string &lexeme, int line) {
    assert(!lexeme.empty());
    assert(line >= 0);
    return add_symbol(lexeme, line, glexemetype::LEXEME_LITERAL, gsymboltype::SYMBOL_TERMINAL);
}

const std::shared_ptr<gsymbol> &ggrammar::regex_symbol(const std::string &lexeme, int line) {
    assert(!lexeme.empty());
    assert(line >= 0);
    return add_symbol(lexeme, line, glexemetype::LEXEME_REGULAR_EXPRESSION, gsymboltype::SYMBOL_TERMINAL);
}

const std::shared_ptr<gsymbol> &ggrammar::add_symbol(const std::string &lexeme, int line, glexemetype lexeme_type, gsymboltype symbol_type) {
    assert(!lexeme.empty());
    assert(line >= 0);
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   add_symbol()\n";
    std::vector<std::shared_ptr<gsymbol>>::const_iterator i = m_symbols.begin();
    while (i != m_symbols.end() && !i->get()->matches(lexeme, symbol_type))
        ++i;
    if (i == m_symbols.end()) {
        std::shared_ptr<gsymbol> symbol = std::make_shared<gsymbol>(gsymbol(lexeme));
        symbol->set_line(line);
        symbol->set_lexeme_type(lexeme_type);
        symbol->set_symbol_type(symbol_type);
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "  create   new SYMBOL      ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*symbol << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*symbol) << ") ";
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->index() << " ";
        /*debug*/ m_log.out << symbol->lexeme() << " ";
        /*debug*/ m_log.out << m_log.creset << "\n";
        m_symbols.push_back(std::move(symbol));
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "  move->   .m_symbols.back ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_symbols.back() << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_symbols.back()) << ") ";
        /*debug*/ m_log.out << "<" << m_symbols.back().use_count() << "> ";
        /*debug*/ m_log.out << m_symbols.back()->index() << " ";
        /*debug*/ m_log.out << m_symbols.back()->lexeme();
        /*debug*/ m_log.out << m_log.creset << "\n";
        return m_symbols.back();
    }
    assert(i->get());
    assert(i->get()->symbol_type() == symbol_type);
    /*debug*/ m_log.out << m_log.cggram;
    /*debug*/ m_log.out << "   found   SYMBOL ";
    /*debug*/ m_log.out << m_log.cwhite;
    /*debug*/ m_log.out << (void *)&*i << "(";
    /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*i) << ") ";
    /*debug*/ m_log.out << "<" << i->use_count() << "> ";
    /*debug*/ m_log.out << i->get()->index() << " ";
    /*debug*/ m_log.out << i->get()->lexeme() << " ";
    /*debug*/ m_log.out << m_log.creset << "\n";
    return *i;
}

const std::shared_ptr<gproduction> &ggrammar::add_production(const std::shared_ptr<gsymbol> &symbol, int line) {
    assert(symbol);
    assert(line > 0);
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   add_production()\n";
    if (m_productions.empty()) {
        assert(m_start_symbol);
        std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), m_start_symbol, 0, 0, nullptr);
        /*debug*/ m_log.out << m_log.cggram << "  create   new PRODUC ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*production) << ") ";
        /*debug*/ m_log.out << "<" << production.use_count() << ">\n";
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "            on SYMBOL ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << &*production.get()->symbol() << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*production.get()->symbol()) << ") ";
        /*debug*/ m_log.out << "<" << production.get()->symbol().use_count() << "> ";
        /*debug*/ m_log.out << production.get()->symbol()->index() << " ";
        /*debug*/ m_log.out << production.get()->symbol()->lexeme() << "\n";
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "            w/ ACTION ";
        /*debug*/ m_log.out << m_log.cwhite;
        /*debug*/ m_log.out << production.get()->action() << " ";
        /*debug*/ m_log.out << (production.get()->action() ? "<" : "");
        /*debug*/ m_log.out << (production.get()->action() ? std::to_string(production.get()->action().use_count()) : "");
        /*debug*/ m_log.out << (production.get()->action() ? "> " : "");
        /*debug*/ m_log.out << (production.get()->action() ? std::to_string(production.get()->action()->index()) : "");
        /*debug*/ m_log.out << (production.get()->action() ? " " : "");
        /*debug*/ m_log.out << (production.get()->action() ? production.get()->action()->identifier() : "");
        /*debug*/ m_log.out << m_log.creset << "\n";
        production->append_symbol(symbol);
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "    push   arg SYMBOL " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*symbol << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*symbol) << ") ";
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->index() << " ";
        /*debug*/ m_log.out << symbol->lexeme() << " ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        /*debug*/ m_log.out << "            to PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*production) << ") ";
        /*debug*/ m_log.out << "<" << production.use_count() << ">\n";
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "           chk SYMBOL " << m_log.cwhite;
        /*debug*/ m_log.out << &*production->symbols().back() << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*production->symbols().back()) << ") ";
        /*debug*/ m_log.out << "<" << production->symbols().back().use_count() << "> ";
        /*debug*/ m_log.out << production->symbols().back()->index() << " ";
        /*debug*/ m_log.out << production->symbols().back()->lexeme() << " ";
        /*debug*/ m_log.out << m_log.creset << "\n";
        m_start_symbol->append_production(production);
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "    push   new PRODUC " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*production) << ") ";
        /*debug*/ m_log.out << "<" << production.use_count() << ">\n";
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "            to START  " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_start_symbol << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_start_symbol) << ") ";
        /*debug*/ m_log.out << "<" << m_start_symbol.use_count() << "> ";
        /*debug*/ m_log.out << m_start_symbol->index() << " ";
        /*debug*/ m_log.out << m_start_symbol->lexeme() << " ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        /*debug*/ m_log.out << "           chk START  " << m_log.cwhite;
        /*debug*/ m_log.out << &*m_start_symbol->productions().back() << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_start_symbol->productions().back()) << ") ";
        /*debug*/ m_log.out << "<" << m_start_symbol->productions().back().use_count() << "> ";
        /*debug*/ m_log.out << m_log.creset << "\n";
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "  move->   .m_productions " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*production << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*production) << ") ";
        /*debug*/ m_log.out << "<" << production.use_count() << "> ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        m_productions.push_back(std::move(production));
        /*debug*/ m_log.out << m_log.cggram;
        /*debug*/ m_log.out << "           chk PRODUC     " << m_log.cwhite;
        /*debug*/ m_log.out << (void *)&*m_productions.back() << "(";
        /*debug*/ m_log.out << reinterpret_cast<std::uintptr_t>(&*m_productions.back()) << ") ";
        /*debug*/ m_log.out << "<" << m_productions.back().use_count() << "> ";
        /*debug*/ m_log.out << m_log.cggram << "\n";
        return m_productions.back();
    }
    std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), symbol, line, -1, nullptr);
    /*debug*/ m_log.out << m_log.cerror << "TODO comment" << m_log.creset << "\n";
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

const std::shared_ptr<gaction> &ggrammar::add_action(const std::string &identifier) {
    assert(!identifier.empty());
    /*debug*/ m_log.out << m_log.cmagenta;
    /*debug*/ m_log.out << "    call   add_action()\n";
    std::vector<std::shared_ptr<gaction>>::const_iterator i = m_actions.begin();
    while (i != m_actions.end() && i->get()->identifier() != identifier)
        ++i;
    if (i == m_actions.end()) {
        int index = int(m_actions.size());
        std::shared_ptr<gaction> action = std::make_shared<gaction>(index, identifier);
        m_actions.push_back(std::move(action));
        return m_actions.back();
    }
    return *i;
}
