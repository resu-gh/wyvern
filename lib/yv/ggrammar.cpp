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
      m_error_symbol(nullptr),
      m_log("yyv", "ggram", 135) {
    m_start_symbol = add_symbol(".start", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
    /*debug*/ m_log.set_fun("constructor");
    /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_start_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_start_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_start_symbol->microdump() << "\n";

    m_end_symbol = add_symbol(".end", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_END);
    /*debug*/ m_log.set_fun("constructor");
    /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_end_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_end_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_end_symbol->microdump() << "\n";

    m_error_symbol = add_symbol("error", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NULL);
    /*debug*/ m_log.set_fun("constructor");
    /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_error_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_error_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_error_symbol->microdump() << "\n";
}

const std::string &ggrammar::identifier() const {
    return m_identifier;
}

std::vector<std::shared_ptr<gsymbol>> &ggrammar::symbols() {
    return m_symbols;
}

std::vector<std::shared_ptr<gproduction>> &ggrammar::productions() {
    return m_productions;
}

std::vector<std::shared_ptr<gaction>> &ggrammar::actions() {
    return m_actions;
}

const std::vector<xtoken> &ggrammar::whitespace_tokens() {
    return m_whitespace_tokens;
}

const std::shared_ptr<gsymbol> &ggrammar::start_symbol() const {
    return m_start_symbol;
}

const std::shared_ptr<gsymbol> &ggrammar::end_symbol() const {
    return m_end_symbol;
}

const std::shared_ptr<gsymbol> &ggrammar::error_symbol() const {
    return m_error_symbol;
}

ggrammar &ggrammar::grammar(const std::string &identifier) {
    m_identifier = identifier;
    return *this;
}

ggrammar &ggrammar::left(int line) {
    /*debug*/ m_log.set_fun("left");

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_LEFT;
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_precedence ";
    /*debug*/ m_log.out << m_log.chl << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";
    m_active_symbol = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    return *this;
}

ggrammar &ggrammar::right(int line) {
    /*debug*/ m_log.set_fun("right");

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_RIGHT;
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_precedence ";
    /*debug*/ m_log.out << m_log.chl << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";
    m_active_symbol = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    return *this;
}

ggrammar &ggrammar::none(int line) {
    /*debug*/ m_log.set_fun("none");

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NONE;
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_precedence ";
    /*debug*/ m_log.out << m_log.chl << m_precedence << " -> ";
    ++m_precedence;
    /*debug*/ m_log.out << m_log.cwhite << m_precedence << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";
    m_active_symbol = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    return *this;
}

ggrammar &ggrammar::whitespace(int line) {
    /*debug*/ m_log.set_fun("whitespace");

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = true;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";
    m_active_symbol = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    return *this;
}

ggrammar &ggrammar::precedence(int line) {
    /*debug*/ m_log.set_fun("precedence");

    assert(m_active_symbol);
    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    if (m_active_symbol) {
        /*debug*/ m_log.trace(1) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive != nullptr\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
        /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
        m_active_precedence_directive = true;
        /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";
    }

    return *this;
}

ggrammar &ggrammar::production(const std::string &identifier, int line) {
    /*debug*/ m_log.set_fun("produc");

    assert(!identifier.empty());

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set..") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";
    m_active_symbol = non_terminal_symbol(identifier, line);
    /*debug*/ m_log.set_fun("produc");
    /*debug*/ m_log.trace(1) << m_log.op("..set") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    return *this;
}

ggrammar &ggrammar::end_production() {
    /*debug*/ m_log.set_fun("end_produc");

    assert(m_active_symbol);

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << " -> ";
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.out << m_log.cwhite << m_associativity << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << " -> ";
    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_whitespace_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

    /*debug*/ m_log.trace(1) << m_log.op("set..") << ".m_active_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";
    m_active_symbol = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_symbol.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_symbol ? m_active_symbol->microdump() : "nullptr") << "\n";

    return *this;
}

ggrammar &ggrammar::error(int line) {
    /*debug*/ m_log.set_fun("error");

    assert(line >= 1);

    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity == ASSOCIATE_NULL (case 1)\n";

        const std::shared_ptr<gsymbol> &symbol = error_symbol();
        /*debug*/ m_log.trace(1) << m_log.op("get") << "symbol <- .error_symbol()\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->microdump() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "symbol.associativity ";
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << " -> ";
        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.cwhite << symbol->associativity() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "symbol.precedence ";
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << " -> ";
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cwhite << symbol->precedence() << "\n";

    } else if (m_active_symbol) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 2)\n";

        if (!m_active_production) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 3)\n";

            /*debug*/ m_log.trace(1) << m_log.op("set..") << ".m_active_production\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.set_fun("error");
            /*debug*/ m_log.trace(0) << m_log.op("..set") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
        }

        m_active_production->append_symbol(error_symbol());
        /*debug*/ m_log.trace(1) << m_log.op("push") << ".m_active_production.symbols <- .error_symbol()\n";
        /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.chl;
        /*debug*/ m_log.out << "<" << error_symbol().use_count() << "> ";
        /*debug*/ m_log.out << error_symbol()->microdump() << "\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_active_production->symbols().back().use_count() << "> ";
        /*debug*/ m_log.out << m_active_production->symbols().back()->microdump() << "\n";
    }

    return *this;
}

ggrammar &ggrammar::literal(const std::string &literal, int line) {
    /*debug*/ m_log.set_fun("literal");

    assert(!literal.empty());
    assert(line >= 0);
    assert(m_active_whitespace_directive || m_associativity != gsymbolassoc::ASSOCIATE_NULL || m_active_symbol);

    if (m_active_whitespace_directive) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_whitespace_directive == true (case 1)\n";

        m_whitespace_tokens.push_back(xtoken(xtokentype::TOKEN_LITERAL, 0, 0, nullptr, literal));
        /*debug*/ m_log.trace(1) << m_log.op("push") << ".m_whitespace_tokens <- new token literal\n";
        /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.chl;
        /*debug*/ m_log.out << m_whitespace_tokens.back().lexeme() << "\n";

    } else if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity != ASSOCIATE_NULL (case 2)\n";

        std::shared_ptr<gsymbol> symbol = literal_symbol(literal, line);
        /*debug*/ m_log.set_fun("literal");
        /*debug*/ m_log.trace(0) << m_log.op("get") << "(symbol) <- literal_symbol()\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->microdump() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).associativity ";
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << " -> ";
        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.cwhite << symbol->associativity() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).precedence ";
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << " -> ";
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cwhite << symbol->precedence() << "\n";

    } else if (m_active_symbol) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 3)\n";

        if (!m_active_production) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 4)\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.set_fun("literal");
            /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_active_production\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->microdump() << "\n";
        }
        if (m_active_precedence_directive) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true (case 5)\n";

            m_active_production->set_precedence_symbol(literal_symbol(literal, line));
            /*debug*/ m_log.set_fun("literal");
            /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_active_production.precedence_symbol\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << " <- literal_symbol()\n ";
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production->precedence_symbol().use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->precedence_symbol()->microdump() << "\n";

            /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
            /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
            m_active_precedence_directive = false;
            /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";
        } else {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false (case 6)\n";

            m_active_production->append_symbol(literal_symbol(literal, line));
            /*debug*/ m_log.set_fun("literal");
            /*debug*/ m_log.trace(0) << m_log.op("push") << ".m_active_production.symbols <- literal_symbol()\n";
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production->symbols().back().use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->symbols().back()->microdump() << "\n";
        }
    }

    return *this;
}

ggrammar &ggrammar::regex(const std::string &regex, int line) {
    /*debug*/ m_log.set_fun("regex");

    assert(!regex.empty());
    assert(line >= 0);
    assert(m_active_whitespace_directive || m_associativity != gsymbolassoc::ASSOCIATE_NULL || m_active_symbol);

    if (m_active_whitespace_directive) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_whitespace_directive == true (case 1)\n";

        m_whitespace_tokens.push_back(xtoken(xtokentype::TOKEN_REGULAR_EXPRESSION, 0, 0, nullptr, regex));
        /*debug*/ m_log.trace(1) << m_log.op("push") << ".m_whitespace_tokens <- new TOKEN\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl << regex << "\n";

    } else if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity != ASSOCIATE_NULL (case 2)\n";

        std::shared_ptr<gsymbol> symbol = regex_symbol(regex, line);
        /*debug*/ m_log.set_fun("regex");
        /*debug*/ m_log.trace(0) << m_log.op("get") << "(symbol) (temp) <- .regex_symbol()\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->microdump() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).associativity ";
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << " -> ";
        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.cwhite << symbol->associativity() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).precedence ";
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << " -> ";
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cwhite << symbol->precedence() << "\n";

    } else if (m_active_symbol) {
        /*debug*/ m_log.trace(0) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 3)\n";

        if (!m_active_production) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 4)\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.set_fun("regex");
            /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_active_production\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->microdump() << "\n";
        }
        if (m_active_precedence_directive) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true (case 5)\n";

            m_active_production->set_precedence_symbol(regex_symbol(regex, line));
            /*debug*/ m_log.set_fun("regex");
            /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_active_production.precedence_symbol\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << " <- regex_symbol()\n ";
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production->precedence_symbol().use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->precedence_symbol()->microdump() << "\n";

            /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
            /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
            m_active_precedence_directive = false;
            /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

        } else {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false (case 6)\n";

            m_active_production->append_symbol(regex_symbol(regex, line));
            /*debug*/ m_log.set_fun("regex");
            /*debug*/ m_log.trace(0) << m_log.op("push") << ".m_active_production.symbols <- regex_symbol()\n";
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production->symbols().back().use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->symbols().back()->microdump() << "\n";
        }
    }

    return *this;
}

ggrammar &ggrammar::identifier(const std::string &identifier, int line) {
    /*debug*/ m_log.set_fun("identifier");

    assert(!identifier.empty());
    assert(line >= 0);
    assert(m_active_symbol || m_associativity != gsymbolassoc::ASSOCIATE_NULL);

    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.trace(1) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity == ASSOCIATE_NULL (case 1)\n";

        std::shared_ptr<gsymbol> symbol = non_terminal_symbol(identifier, line);
        /*debug*/ m_log.set_fun("identifier");
        /*debug*/ m_log.trace(0) << m_log.op("get") << "(symbol) (temp) <- .non_terminal_symbol()\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->microdump() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).associativity ";
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << " -> ";
        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.cwhite << symbol->associativity() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).precedence ";
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << " -> ";
        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.cwhite << symbol->precedence() << "\n";

    } else if (m_active_symbol) {
        /*debug*/ m_log.trace(1) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 2)\n";

        if (!m_active_production) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 3)\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.set_fun("identifier");
            /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_active_production\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->microdump() << "\n";
        }
        if (m_active_precedence_directive) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true (case 4)\n";

            m_active_production->set_precedence_symbol(non_terminal_symbol(identifier, line));
            /*debug*/ m_log.set_fun("identifier");
            /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_active_production.precedence_symbol\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << " <- non_terminal_symbol()\n ";
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production->precedence_symbol().use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->precedence_symbol()->microdump() << "\n";

            /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_precedence_directive ";
            /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << " -> ";
            m_active_precedence_directive = false;
            /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << " -> ";

        } else {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false (case 5)\n";

            m_active_production->append_symbol(non_terminal_symbol(identifier, line));
            /*debug*/ m_log.set_fun("identifier");
            /*debug*/ m_log.trace(0) << m_log.op("push") << ".m_active_production.symbols\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << "<- non_terminal_symbol()\n";
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production->symbols().back().use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->symbols().back()->microdump() << "\n";
        }
    }
    return *this;
}

ggrammar &ggrammar::action(const std::string &identifier, int line) {
    /*debug*/ m_log.set_fun("action");

    assert(!identifier.empty());
    assert(line >= 1);
    assert(m_active_symbol);

    if (m_active_symbol) {
        /*debug*/ m_log.trace(1) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 1)\n";

        if (!m_active_production) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 2)\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.set_fun("action");
            /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_active_production\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->microdump() << "\n";
        }

        m_active_production->set_action(add_action(identifier));
        /*debug*/ m_log.set_fun("action");
        /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_active_production.action\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_active_production->action().use_count() << "> ";
        /*debug*/ m_log.out << m_active_production->action()->microdump() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << ".m_active_production\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
        /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
        m_active_production = nullptr;
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
        /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    }

    return *this;
}

ggrammar &ggrammar::end_expression(int line) {
    /*debug*/ m_log.set_fun("end_expr");

    assert(line >= 1);

    // if active_symbol but not active_production
    // an empty production is being specified
    // the nil action marks the end of a production
    // for which no symbols have been specified
    if (m_active_symbol) {
        /*debug*/ m_log.trace(1) << m_log.op("if");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr (case 1)\n";

        if (!m_active_production) {
            /*debug*/ m_log.trace(1) << m_log.op("if");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr (case 2)\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.set_fun("end_expr");
            /*debug*/ m_log.trace(0) << m_log.op("test") << ".m_active_production\n";
            /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
            /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
            /*debug*/ m_log.out << m_active_production->microdump() << "\n";
        }
    }

    /*debug*/ m_log.trace(0) << m_log.op("set") << ".m_active_production\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";
    m_active_production = nullptr;
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_active_production.use_count() << "> ";
    /*debug*/ m_log.out << (m_active_production ? m_active_production->microdump() : "nullptr") << "\n";

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
    /*debug*/ m_log.set_fun("add_symbol");

    assert(!lexeme.empty());
    assert(line >= 0);

    std::vector<std::shared_ptr<gsymbol>>::const_iterator i = m_symbols.begin();

    while (i != m_symbols.end() && !i->get()->matches(lexeme, symbol_type))
        ++i;

    if (i == m_symbols.end()) {
        std::shared_ptr<gsymbol> symbol = std::make_shared<gsymbol>(gsymbol(lexeme));
        /*debug*/ m_log.trace(0) << m_log.op(" new") << "(symbol) (temp) <- SYMBOL\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->microdump() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).line " << m_log.chl;
        /*debug*/ m_log.out << symbol->line() << " -> " << m_log.cwhite;
        symbol->set_line(line);
        /*debug*/ m_log.out << symbol->line() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).lexeme_type " << m_log.chl;
        /*debug*/ m_log.out << symbol->lexeme_type() << " -> " << m_log.cwhite;
        symbol->set_lexeme_type(lexeme_type);
        /*debug*/ m_log.out << symbol->lexeme_type() << "\n";

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(symbol).symbol_type " << m_log.chl;
        /*debug*/ m_log.out << symbol->symbol_type() << " -> " << m_log.cwhite;
        symbol->set_symbol_type(symbol_type);
        /*debug*/ m_log.out << symbol->symbol_type() << "\n";

        m_symbols.push_back(std::move(symbol));
        /*debug*/ m_log.trace(1) << m_log.op("move") << ".m_symbols <- new SYMBOL\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_symbols.back().use_count() << "> ";
        /*debug*/ m_log.out << m_symbols.back()->microdump() << "\n";

        return m_symbols.back();
    }

    assert(i->get());
    assert(i->get()->symbol_type() == symbol_type);

    /*debug*/ m_log.trace(0) << m_log.op("found") << "SYMBOL\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << i->use_count() << "> ";
    /*debug*/ m_log.out << i->get()->microdump() << "\n";

    return *i;
}

const std::shared_ptr<gproduction> &ggrammar::add_production(const std::shared_ptr<gsymbol> &symbol, int line) {
    /*debug*/ m_log.set_fun("add_produc");

    assert(symbol);
    assert(line > 0);

    if (m_productions.empty()) {
        m_log.trace(0) << m_log.op("if") << m_log.ccyan << ".m_productions is empty\n";

        assert(m_start_symbol);

        std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), m_start_symbol, 0, 0, nullptr);
        /*debug*/ m_log.trace(0) << m_log.op("new") << "PRODUC from .m_start_symbol\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "<" << production.use_count() << "> ";
        /*debug*/ m_log.out << production->microdump() << "\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "on <" << production->symbol().use_count() << "> ";
        /*debug*/ m_log.out << production->symbol()->microdump() << "\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "w/ <" << production->action().use_count() << "> ";
        /*debug*/ m_log.out << (production->action() ? production->action()->microdump() : "nullptr") << "\n";

        production->append_symbol(symbol);
        /*debug*/ m_log.trace(1) << m_log.op("push") << "new PRODUC <- symbol (param)\n";
        /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.chl;
        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
        /*debug*/ m_log.out << symbol->microdump() << "\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << production->symbols().back().use_count() << "> ";
        /*debug*/ m_log.out << production->symbols().back()->microdump() << "\n";

        m_start_symbol->append_production(production);
        /*debug*/ m_log.trace(1) << m_log.op("push") << ".m_start_symbol <- new PRODUC\n";
        /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.chl;
        /*debug*/ m_log.out << "<" << production.use_count() << "> ";
        /*debug*/ m_log.out << production->microdump() << "\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_start_symbol->productions().back().use_count() << "> ";
        /*debug*/ m_log.out << m_start_symbol->productions().back()->microdump() << "\n";

        m_productions.push_back(std::move(production));
        /*debug*/ m_log.trace(1) << m_log.op("move") << ".m_productions <- new PRODUC\n";
        /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_productions.back().use_count() << "> ";
        /*debug*/ m_log.out << m_productions.back()->microdump() << "\n";

        return m_productions.back();
    }

    std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), symbol, line, -1, nullptr);
    /*debug*/ m_log.trace(0) << m_log.op("new") << "PRODUC from symbol (param)\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "<" << production.use_count() << "> ";
    /*debug*/ m_log.out << production->microdump() << "\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "on <" << production->symbol().use_count() << "> ";
    /*debug*/ m_log.out << production->symbol()->microdump() << "\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << "w/ <" << production->action().use_count() << "> ";
    /*debug*/ m_log.out << (production->action() ? production->action()->microdump() : "nullptr") << "\n";

    symbol->append_production(production);
    /*debug*/ m_log.trace(1) << m_log.op("push") << "symbol (param) <- new PRODUC\n";
    /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.chl;
    /*debug*/ m_log.out << "<" << production.use_count() << "> ";
    /*debug*/ m_log.out << production->microdump() << "\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << symbol->productions().back().use_count() << "> ";
    /*debug*/ m_log.out << symbol->productions().back()->microdump() << "\n";

    m_productions.push_back(std::move(production));
    /*debug*/ m_log.trace(1) << m_log.op("move") << ".m_productions <- new PRODUC\n";
    /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.cwhite;
    /*debug*/ m_log.out << "<" << m_productions.back().use_count() << "> ";
    /*debug*/ m_log.out << m_productions.back()->microdump() << "\n";

    return m_productions.back();
}

const std::shared_ptr<gaction> &ggrammar::add_action(const std::string &identifier) {
    /*debug*/ m_log.set_fun("add_action");

    assert(!identifier.empty());

    std::vector<std::shared_ptr<gaction>>::const_iterator i = m_actions.begin();

    while (i != m_actions.end() && i->get()->identifier() != identifier)
        ++i;

    if (i == m_actions.end()) {
        int index = int(m_actions.size());

        std::shared_ptr<gaction> action = std::make_shared<gaction>(index, identifier);
        /*debug*/ m_log.trace(0) << m_log.op("new") << "ACTION from identifier (param)\n";
        /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/ m_log.out << "<" << action.use_count() << "> ";
        /*debug*/ m_log.out << action->microdump() << "\n";

        m_actions.push_back(std::move(action));
        /*debug*/ m_log.trace(1) << m_log.op("move") << ".m_actions <- new ACTION\n";
        /*debug*/ m_log.trace(1) << m_log.op("check") << m_log.cwhite;
        /*debug*/ m_log.out << "<" << m_actions.back().use_count() << "> ";
        /*debug*/ m_log.out << m_actions.back()->microdump() << "\n";

        return m_actions.back();
    }

    /*debug*/ m_log.trace(0) << m_log.op("found") << "ACTION " << m_log.chl;
    /*debug*/ m_log.out << "<" << i->use_count() << "> ";
    /*debug*/ m_log.out << i->get()->microdump() << "\n";

    return *i;
}

void ggrammar::dump() const {
    const std::string cc = "\033[38;5;240m";
    const std::string cc2 = "\033[38;5;245m";
    m_log.out << "\n";
    m_log.out << m_log.cred << "IDENTIFIER: ";
    m_log.out << m_log.cmagenta << m_identifier;
    m_log.out << m_log.creset << "\n\n";
    m_log.out << m_log.cred << "START STATE: ";
    m_log.out << m_log.cgreen << m_start_symbol->index() << " ";
    m_log.out << m_log.ccyan << (void*)&*m_start_symbol << " <";
    m_log.out << m_log.ccyan << m_start_symbol.use_count() << "> ";
    m_log.out << m_log.cmagenta << m_start_symbol->lexeme() << " ";
    m_log.out << cc;
    m_log.out << m_start_symbol->precedence() << " ";
    m_log.out << m_start_symbol->symbol_type() << " ";
    m_log.out << m_start_symbol->lexeme_type() << " ";
    m_log.out << m_start_symbol->associativity() << " ";
    if (m_start_symbol->productions().size()) {
        m_log.out << "[";
        m_log.out << cc2;
        for (auto p : m_start_symbol->productions()) {
            m_log.out << p->index();
            if (&*p != &*(m_start_symbol->productions().end() - 1)->get())
                m_log.out << cc << "," << cc2;
        }
        m_log.out << cc;
        m_log.out << "]";
    }
    m_log.out << m_log.creset << "\n\n";
    m_log.out << m_log.cred << "END STATE: ";
    m_log.out << m_log.cgreen << m_end_symbol->index() << " ";
    m_log.out << m_log.ccyan << (void*)&*m_end_symbol << " <";
    m_log.out << m_log.ccyan << m_end_symbol.use_count() << "> ";
    m_log.out << m_log.cmagenta << m_end_symbol->lexeme() << " ";
    m_log.out << cc;
    m_log.out << m_end_symbol->precedence() << " ";
    m_log.out << m_end_symbol->symbol_type() << " ";
    m_log.out << m_end_symbol->lexeme_type() << " ";
    m_log.out << m_end_symbol->associativity() << " ";
    if (m_end_symbol->productions().size()) {
        m_log.out << "[";
        m_log.out << cc2;
        for (auto p : m_end_symbol->productions()) {
            m_log.out << p->index();
            if (&*p != &*(m_end_symbol->productions().end() - 1)->get())
                m_log.out << cc << "," << cc2;
        }
        m_log.out << cc;
        m_log.out << "]";
    }
    m_log.out << m_log.creset << "\n\n";
    m_log.out << m_log.cred << "ERROR STATE: ";
    m_log.out << m_log.cgreen << m_error_symbol->index() << " ";
    m_log.out << m_log.ccyan << (void*)&*m_error_symbol << " <";
    m_log.out << m_log.ccyan << m_error_symbol.use_count() << "> ";
    m_log.out << m_log.cmagenta << m_error_symbol->lexeme() << " ";
    m_log.out << cc;
    m_log.out << m_error_symbol->precedence() << " ";
    m_log.out << m_error_symbol->symbol_type() << " ";
    m_log.out << m_error_symbol->lexeme_type() << " ";
    m_log.out << m_error_symbol->associativity() << " ";
    if (m_error_symbol->productions().size()) {
        m_log.out << "[";
        m_log.out << cc2;
        for (auto p : m_error_symbol->productions()) {
            m_log.out << p->index();
            if (&*p != &*(m_error_symbol->productions().end() - 1)->get())
                m_log.out << cc << "," << cc2;
        }
        m_log.out << cc;
        m_log.out << "]";
    }
    m_log.out << m_log.creset << "\n\n";
    m_log.out << m_log.cred << "SYMBOLS:\n" << m_log.creset;
    for (auto s : m_symbols) {
        m_log.out << m_log.cgreen << s->index() << " ";
        m_log.out << m_log.ccyan << (void*)&*s << " <";
        m_log.out << m_log.ccyan << s.use_count() << "> ";
        m_log.out << m_log.cmagenta << s->lexeme() << " ";
        m_log.out << cc;
        m_log.out << s->precedence() << " ";
        m_log.out << s->symbol_type() << " ";
        m_log.out << s->lexeme_type() << " ";
        m_log.out << s->associativity() << " ";
        if (s->productions().size()) {
            m_log.out << "[";
            m_log.out << cc2;
            for (auto p : s->productions()) {
                m_log.out << p->index();
                if (&*p != &*(s->productions().end() - 1)->get())
                    m_log.out << cc << "," << cc2;
            }
            m_log.out << cc;
            m_log.out << "]";
        }
        m_log.out << "\n";
    }
    m_log.out << m_log.creset << "\n";
    m_log.out << m_log.cred << "PRODUCTIONS:\n" << m_log.creset;
    for (auto p : m_productions) {
        if (p->index() < 10)
            m_log.out << " ";
        m_log.out << m_log.cgreen << p->index() << " ";
        m_log.out << m_log.ccyan << (void*)&*p << " <";
        m_log.out << m_log.ccyan << p.use_count() << "> ";
        m_log.out << m_log.cmagenta << p->symbol()->lexeme() << " ";
        m_log.out << cc;
        m_log.out << "[";
        m_log.out << cc2;
        for (auto es : p->symbols()) {
            m_log.out << es->lexeme();
            if (&*es != &*(p->symbols().end() - 1)->get())
                m_log.out << cc << "," << cc2;
        }
        m_log.out << cc;
        m_log.out << "] ";
        if (p->action())
            m_log.out << "(" << p->action()->identifier() << ") ";
        m_log.out << "\n";
    }
    m_log.out << m_log.creset << "\n";
    m_log.out << m_log.cred << "WHITESPACE TOKENS:\n" << m_log.creset;
    for (auto t : m_whitespace_tokens) {
        m_log.out << m_log.cgreen << t.line() << " ";
        m_log.out << m_log.ccyan << "." << (void*)&t << " ";
        m_log.out << m_log.cmagenta << t.lexeme() << " ";
        m_log.out << cc;
        m_log.out << t.type();
        m_log.out << "\n";
    }
    m_log.out << m_log.creset << "\n";
    m_log.out << m_log.cred << "ACTIONS:\n" << m_log.creset;
    for (auto a : m_actions) {
        m_log.out << m_log.cgreen << a->index() << " ";
        m_log.out << m_log.ccyan << (void*)&*a << " <";
        m_log.out << m_log.ccyan << a.use_count() << "> ";
        m_log.out << m_log.cmagenta << a->identifier() << " ";
        m_log.out << "\n";
    }
    m_log.out << m_log.creset << "\n";
}
