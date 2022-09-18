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
    /*debug*/ std::string h = m_log.hook("constructor");

    m_start_symbol = add_symbol(".start", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
    /*debug*/ m_log.htrace(h, "m_start_symbol (shared owned) (ref w/ m_symbols)") << "\n";
    /*debug*/ m_start_symbol->json(0, false, 0, false, m_start_symbol.use_count());

    m_end_symbol = add_symbol(".end", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_END);
    /*debug*/ m_log.htrace(h, "m_end_symbol (shared owned) (ref w/ m_symbols)") << "\n";
    /*debug*/ m_end_symbol->json(0, false, 0, false, m_end_symbol.use_count());

    m_error_symbol = add_symbol("error", 0, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NULL);
    /*debug*/ m_log.htrace(h, "m_error_symbol (shared owned) (ref w/ m_symbols)") << "\n";
    /*debug*/ m_error_symbol->json(0, false, 0, false, m_error_symbol.use_count());
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
    /*debug*/ std::string h = m_log.hook("left");

    m_associativity = gsymbolassoc::ASSOCIATE_LEFT;
    /*debug*/ m_log.htrace(h, "set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << "\n";

    ++m_precedence;
    /*debug*/ m_log.out << m_log.op("set") << ".m_precedence ";
    /*debug*/ m_log.out << m_log.chl << m_precedence << "\n";

    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << "\n";

    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";

    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_symbol ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    return *this;
}

ggrammar &ggrammar::right(int line) {
    /*debug*/ std::string h = m_log.hook("right");

    m_associativity = gsymbolassoc::ASSOCIATE_RIGHT;
    /*debug*/ m_log.htrace(h, "set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << "\n";

    ++m_precedence;
    /*debug*/ m_log.out << m_log.op("set") << ".m_precedence ";
    /*debug*/ m_log.out << m_log.chl << m_precedence << "\n";

    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << "\n";

    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";

    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_symbol ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    return *this;
}

ggrammar &ggrammar::none(int line) {
    /*debug*/ std::string h = m_log.hook("none");

    m_associativity = gsymbolassoc::ASSOCIATE_NONE;
    /*debug*/ m_log.htrace(h, "set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << "\n";

    ++m_precedence;
    /*debug*/ m_log.out << m_log.op("set") << ".m_precedence ";
    /*debug*/ m_log.out << m_log.chl << m_precedence << "\n";

    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << "\n";

    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";

    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_symbol ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    return *this;
}

ggrammar &ggrammar::whitespace(int line) {
    /*debug*/ std::string h = m_log.hook("whitespace");

    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.htrace(h, "set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << "\n";

    m_active_whitespace_directive = true;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << "\n";

    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";

    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_symbol ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    return *this;
}

ggrammar &ggrammar::precedence(int line) {
    /*debug*/ std::string h = m_log.hook("precedence");

    assert(m_active_symbol);
    /*debug*/ m_log.htrace(h, "get current m_active_symbol") << "\n";
    /*debug*/ m_active_symbol->json(0, false, 0, false, m_active_symbol.use_count());

    if (m_active_symbol) {

        m_active_precedence_directive = true;
        /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
        /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";
    }

    return *this;
}

ggrammar &ggrammar::production(const std::string &identifier, int line) {
    /*debug*/ std::string h = m_log.hook("production");

    assert(!identifier.empty());

    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.htrace(h, "set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << "\n";

    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << "\n";

    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";

    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    m_active_symbol = non_terminal_symbol(identifier, line);
    /*debug*/ m_log.htrace(h, "set") << ".m_active_symbol\n";
    /*debug*/ m_active_symbol->json(0, false, 0, false, m_active_symbol.use_count());

    return *this;
}

ggrammar &ggrammar::end_production() {
    /*debug*/ std::string h = m_log.hook("end_production");

    assert(m_active_symbol);

    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    /*debug*/ m_log.htrace(h, "set") << ".m_associativity ";
    /*debug*/ m_log.out << m_log.chl << m_associativity << "\n";

    m_active_whitespace_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_whitespace_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_whitespace_directive ? "true" : "false") << "\n";

    m_active_precedence_directive = false;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
    /*debug*/ m_log.out << m_log.chl << (m_active_precedence_directive ? "true" : "false") << "\n";

    m_active_production = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    m_active_symbol = nullptr;
    /*debug*/ m_log.out << m_log.op("set") << ".m_active_symbol ";
    /*debug*/ m_log.out << m_log.chl << "nullptr\n";

    return *this;
}

ggrammar &ggrammar::error(int line) {
    /*debug*/ std::string h = m_log.hook("error");

    assert(line >= 1);

    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity == ASSOCIATE_NULL\n";

        const std::shared_ptr<gsymbol> &symbol = error_symbol();
        /*debug*/ m_log.out << m_log.op("get m_error_symbol (reference) -> symbol (local variable)") << "\n";
        /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) associativity");
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << "\n";

        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) precedence");
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << "\n";

        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

    } else if (m_active_symbol) {
        /*debug*/ m_log.htrace(h, "if (case 2)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr\n";

        /*debug*/ m_active_symbol->json(0, false, 0, false, m_active_symbol.use_count());

        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.op("if (case 3)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.htrace(h, "set") << ".m_active_production\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }

        m_active_production->append_symbol(error_symbol());
        /*debug*/ m_log.out << m_log.op("push m_error_symbol (reference) -> m_active_production.symbols") << "\n";
        /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
    }

    return *this;
}

ggrammar &ggrammar::literal(const std::string &literal, int line) {
    /*debug*/ std::string h = m_log.hook("literal");

    assert(!literal.empty());
    assert(line >= 0);
    assert(m_active_whitespace_directive || m_associativity != gsymbolassoc::ASSOCIATE_NULL || m_active_symbol);

    if (m_active_whitespace_directive) { // TODO FIXME maybe this case is not possible! (gparser.match_whitespace_statement())
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_whitespace_directive == true\n";

        m_whitespace_tokens.push_back(xtoken(xtokentype::TOKEN_LITERAL, 0, 0, nullptr, literal));
        /*debug*/ m_log.out << m_log.op("push new xtoken -> m_whitespace_tokens vector") << "\n";
        /*debug*/ m_whitespace_tokens.back().json(0, false, 0, false);

    } else if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.htrace(h, "if (case 2)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity != ASSOCIATE_NULL\n";

        std::shared_ptr<gsymbol> symbol = literal_symbol(literal, line);
        /*debug*/ m_log.htrace(h, "get symbol (variable, new literal)") << "\n";
        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) associativity ");
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << "\n";

        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) precedence ");
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << "\n";

        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

    } else if (m_active_symbol) {
        /*debug*/ m_log.htrace(h, "if (case 3)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr\n";

        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.op("if (case 4)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.htrace(h, "set") << ".m_active_production\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }

        if (m_active_precedence_directive) {
            /*debug*/ m_log.out << m_log.op("if (case 5)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true\n";

            m_active_production->set_precedence_symbol(literal_symbol(literal, line));
            /*debug*/ m_log.htrace(h, "set new literal symbol from literal (param) -> m_active_production.precedence_symbol") << "\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());

            m_active_precedence_directive = false;
            /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
            /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

        } else {
            /*debug*/ m_log.out << m_log.op("if (case 6)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false\n";

            m_active_production->append_symbol(literal_symbol(literal, line));
            /*debug*/ m_log.htrace(h, "push new literal symbol from literal (param) -> m_active_production.symbols") << "\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }
    }

    return *this;
}

ggrammar &ggrammar::regex(const std::string &regex, int line) {
    /*debug*/ std::string h = m_log.hook("regex");

    assert(!regex.empty());
    assert(line >= 0);
    assert(m_active_whitespace_directive || m_associativity != gsymbolassoc::ASSOCIATE_NULL || m_active_symbol);

    if (m_active_whitespace_directive) {
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_whitespace_directive == true\n";

        m_whitespace_tokens.push_back(xtoken(xtokentype::TOKEN_REGULAR_EXPRESSION, 0, 0, nullptr, regex));
        /*debug*/ m_log.out << m_log.op("push new xtoken -> m_whitespace_tokens vector") << "\n";
        /*debug*/ m_whitespace_tokens.back().json(0, false, 0, false);

    } else if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.htrace(h, "if (case 2)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity != ASSOCIATE_NULL\n";

        std::shared_ptr<gsymbol> symbol = regex_symbol(regex, line);
        /*debug*/ m_log.htrace(h, "get symbol (variable, new literal)") << "\n";
        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) associativity ");
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << "\n";

        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) precedence ");
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << "\n";

        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

    } else if (m_active_symbol) {
        /*debug*/ m_log.htrace(h, "if (case 3)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr\n";

        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.op("if (case 4)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.htrace(h, "set") << ".m_active_production\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }

        if (m_active_precedence_directive) {
            /*debug*/ m_log.out << m_log.op("if (case 5)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true\n";

            m_active_production->set_precedence_symbol(regex_symbol(regex, line));
            /*debug*/ m_log.htrace(h, "set new regex symbol from regex (param) -> m_active_production.precedence_symbol") << "\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());

            m_active_precedence_directive = false;
            /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
            /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

        } else {
            /*debug*/ m_log.out << m_log.op("if (case 6)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false\n";

            m_active_production->append_symbol(regex_symbol(regex, line));
            /*debug*/ m_log.htrace(h, "push new regex symbol from regex (param) -> m_active_production.symbols") << "\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }
    }

    return *this;
}

ggrammar &ggrammar::identifier(const std::string &identifier, int line) {
    /*debug*/ std::string h = m_log.hook("identifier");

    assert(!identifier.empty());
    assert(line >= 0);
    assert(m_active_symbol || m_associativity != gsymbolassoc::ASSOCIATE_NULL);

    if (m_associativity != gsymbolassoc::ASSOCIATE_NULL) {
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_associativity != ASSOCIATE_NULL\n";

        std::shared_ptr<gsymbol> symbol = non_terminal_symbol(identifier, line);
        /*debug*/ m_log.htrace(h, "get symbol (variable, new identifier)") << "\n";
        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

        symbol->set_associativity(m_associativity);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) associativity ");
        /*debug*/ m_log.out << m_log.chl << symbol->associativity() << "\n";

        symbol->set_precedence(m_precedence);
        /*debug*/ m_log.out << m_log.op("set symbol (variable) precedence ");
        /*debug*/ m_log.out << m_log.chl << symbol->precedence() << "\n";

        /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

    } else if (m_active_symbol) {
        /*debug*/ m_log.htrace(h, "if (case 2)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr\n";

        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.op("if (case 3)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.htrace(h, "set") << ".m_active_production\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }

        if (m_active_precedence_directive) {
            /*debug*/ m_log.out << m_log.op("if (case 4)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == true\n";

            m_active_production->set_precedence_symbol(non_terminal_symbol(identifier, line));
            /*debug*/ m_log.htrace(h, "set new non-terminal symbol from identifier (param) -> m_active_production.precedence_symbol") << "\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());

            m_active_precedence_directive = false;
            /*debug*/ m_log.out << m_log.op("set") << ".m_active_precedence_directive ";
            /*debug*/ m_log.out << m_log.cwhite << (m_active_precedence_directive ? "true" : "false") << "\n";

        } else {
            /*debug*/ m_log.out << m_log.op("if (case 5)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_precedence_directive == false\n";

            m_active_production->append_symbol(non_terminal_symbol(identifier, line));
            /*debug*/ m_log.htrace(h, "push new non-terminal symbol from identifier (param) -> m_active_production.symbols") << "\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }
    }
    return *this;
}

ggrammar &ggrammar::action(const std::string &identifier, int line) {
    /*debug*/ std::string h = m_log.hook("action");

    assert(!identifier.empty());
    assert(line >= 1);
    assert(m_active_symbol);

    if (m_active_symbol) {
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr\n";

        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.op("if (case 2)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.htrace(h, "set") << ".m_active_production\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
        }

        m_active_production->set_action(add_action(identifier));
        /*debug*/ m_log.htrace(h, "set new action from identifier (param) -> m_active_production.action") << "\n";
        /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());

        m_active_production = nullptr;
        /*debug*/ m_log.out << m_log.op("set") << ".m_active_production ";
        /*debug*/ m_log.out << m_log.chl << "nullptr\n";
    }

    return *this;
}

ggrammar &ggrammar::end_expression(int line) {
    /*debug*/ std::string h = m_log.hook("end_expression");

    assert(line >= 1);

    // if active_symbol but not active_production
    // an empty production is being specified
    // the nil action marks the end of a production
    // for which no symbols have been specified
    if (m_active_symbol) {
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_active_symbol != nullptr\n";

        if (!m_active_production) {
            /*debug*/ m_log.out << m_log.op("if (case 2)");
            /*debug*/ m_log.out << m_log.ccyan << ".m_active_production == nullptr\n";

            m_active_production = add_production(m_active_symbol, line);
            /*debug*/ m_log.htrace(h, "set") << ".m_active_production\n";
            /*debug*/ m_active_production->json(0, false, 0, false, m_active_production.use_count());
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
    /*debug*/ std::string h = m_log.hook("add_symbol");

    assert(!lexeme.empty());
    assert(line >= 0);

    std::vector<std::shared_ptr<gsymbol>>::const_iterator i = m_symbols.begin();

    while (i != m_symbols.end() && !i->get()->matches(lexeme, symbol_type))
        ++i;

    if (i == m_symbols.end()) {
        std::shared_ptr<gsymbol> symbol = std::make_shared<gsymbol>(gsymbol(lexeme));
        /*debug*/ m_log.htrace(h, "new symbol (builded) from lexeme (param)") << "\n";
        /*debug*/ symbol->json(0, true, 0, true, symbol.use_count());

        symbol->set_line(line);
        symbol->set_lexeme_type(lexeme_type);
        symbol->set_symbol_type(symbol_type);

        m_symbols.push_back(std::move(symbol));
        /*debug*/ m_log.out << m_log.op("new symbol -> m_symbols (moved, test)") << "\n";
        /*debug*/ m_symbols.back()->json(0, true, 0, true, m_symbols.back().use_count());

        return m_symbols.back();
    }

    assert(i->get());
    assert(i->get()->symbol_type() == symbol_type);

    /*debug*/ m_log.htrace(h, "symbol (found)") << "\n";
    /*debug*/ i->get()->json(0, true, 0, true, i->use_count());

    return *i;
}

const std::shared_ptr<gproduction> &ggrammar::add_production(const std::shared_ptr<gsymbol> &symbol, int line) {
    /*debug*/ std::string h = m_log.hook("add_production");

    assert(symbol);
    assert(line > 0);

    if (m_productions.empty()) {
        /*debug*/ m_log.htrace(h, "if (case 1)");
        /*debug*/ m_log.out << m_log.ccyan << ".m_productions vector is empty\n";

        assert(m_start_symbol);

        std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), m_start_symbol, 0, 0, nullptr);
        /*debug*/ m_log.out << m_log.op("new production (builded) from m_start_symbol -> production (variable)") << "\n";
        /*debug*/ production->json(0, false, 0, false, production.use_count());

        production->append_symbol(symbol);
        /*debug*/ m_log.out << m_log.op("push symbol (param) -> production (variable) symbols") << "\n";
        /*debug*/ production->json(0, false, 0, false, production.use_count());

        m_start_symbol->append_production(production);
        /*debug*/ m_log.out << m_log.op("push production (variable) -> m_start_symbol.productions") << "\n";
        /*debug*/ m_start_symbol->json(0, false, 0, false, m_start_symbol.use_count());

        m_productions.push_back(std::move(production));
        /*debug*/ m_log.out << m_log.op("move production (variable) -> m_productions vector") << "\n";
        /*debug*/ m_productions.back()->json(0, true, 0, true, m_productions.back().use_count());
    }

    std::shared_ptr<gproduction> production = std::make_shared<gproduction>(int(m_productions.size()), symbol, line, -1, nullptr);
    /*debug*/ m_log.htrace(h, "new production (builded) from symbol (param) -> production (variable)") << "\n";
    /*debug*/ production->json(0, false, 0, false, production.use_count());

    symbol->append_production(production);
    /*debug*/ m_log.out << m_log.op("push production (variable) -> symbol (parameter) productions") << "\n";
    /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());

    m_productions.push_back(std::move(production));
    /*debug*/ m_log.out << m_log.op("move production (variable) -> m_productions vector") << "\n";
    /*debug*/ m_productions.back()->json(0, true, 0, true, m_productions.back().use_count());

    return m_productions.back();
}

const std::shared_ptr<gaction> &ggrammar::add_action(const std::string &identifier) {
    /*debug*/ std::string h = m_log.hook("add_action");

    assert(!identifier.empty());

    std::vector<std::shared_ptr<gaction>>::const_iterator i = m_actions.begin();

    while (i != m_actions.end() && i->get()->identifier() != identifier)
        ++i;

    if (i == m_actions.end()) {
        int index = int(m_actions.size());

        std::shared_ptr<gaction> action = std::make_shared<gaction>(index, identifier);
        /*debug*/ m_log.htrace(h, "variable action = new action (builded) from identifier (param)") << "\n";
        /*debug*/ action->json(0, false, 0, false, action.use_count());

        m_actions.push_back(std::move(action));
        /*debug*/ m_log.out << m_log.op("move action (variable) -> m_actions vector") << "\n";
        /*debug*/ m_actions.back()->json(0, true, 0, true, m_actions.back().use_count());

        return m_actions.back();
    }

    /*debug*/ m_log.htrace(h, "action (found)") << "\n";
    /*debug*/ i->get()->json(0, true, 0, true, i->use_count());

    return *i;
}

void ggrammar::dump(bool compact) const {
    std::string h = m_log.hook("dump");

    m_log.htrace(h, "m_identifier") << m_log.chl << m_identifier << "\n";

    m_log.out << m_log.op("m_start_symbol") << (compact ? "" : "\n");
    m_start_symbol->json(0, false, 0, compact, m_start_symbol.use_count());

    m_log.out << m_log.op("m_end_symbol") << (compact ? "" : "\n");
    m_end_symbol->json(0, false, 0, compact, m_end_symbol.use_count());

    m_log.out << m_log.op("m_error_symbol") << (compact ? "" : "\n");
    m_error_symbol->json(0, false, 0, compact, m_error_symbol.use_count());

    m_log.out << m_log.op("m_symbols") << (m_symbols.empty() ? "empty\n" : "\n");
    for (auto s : m_symbols)
        s->json(0, false, 0, compact, s.use_count());

    m_log.out << m_log.op("m_productions") << (m_productions.empty() ? "empty\n" : "\n");
    for (auto p : m_productions)
        p->json(0, false, 0, compact, p.use_count());

    m_log.out << m_log.op("m_actions") << (m_actions.empty() ? "empty\n" : "\n");
    for (auto a : m_actions)
        a->json(0, false, 0, compact, a.use_count());

    m_log.out << m_log.op("m_whitespace_tokens") << (m_whitespace_tokens.empty() ? "empty\n" : "\n");
    for (auto t : m_whitespace_tokens)
        t.json(0, false, 0, compact);
}
