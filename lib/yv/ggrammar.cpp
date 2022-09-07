#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"
#include "include/gsymbol.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>

ggrammar::ggrammar()
    : m_identifier(),
      m_symbols(),
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
    // debug begin
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "new LEFT ";
    m_log.out << m_log.cyellow << "%left ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    // debug end
    m_associativity = gsymbolassoc::ASSOCIATE_LEFT;
    ++m_precedence;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

ggrammar &ggrammar::right(int line) {
    // debug begin
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "new RIGHT ";
    m_log.out << m_log.cyellow << "%right ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    // debug end
    m_associativity = gsymbolassoc::ASSOCIATE_RIGHT;
    ++m_precedence;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

ggrammar &ggrammar::none(int line) {
    // debug begin
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "new NONE ";
    m_log.out << m_log.cyellow << "%none ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    // debug end
    m_associativity = gsymbolassoc::ASSOCIATE_NONE;
    ++m_precedence;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

ggrammar &ggrammar::whitespace(int line) {
    // debug begin
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "new WHITESPACE ";
    m_log.out << m_log.cyellow << "%whitespace ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    // debug end
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    m_active_whitespace_directive = true;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr; // TODO
    return *this;
}

ggrammar &ggrammar::precedence(int line) {
    // debug begin
    m_log.out << m_log.cmagenta << "[ggrammr] ";
    m_log.out << m_log.cgreen << "new PRECEDENCE ";
    m_log.out << m_log.cyellow << "%precedence ";
    m_log.out << m_log.creset << "[" << line << "]\n";
    // debug end
    assert(m_active_symbol);
    if (m_active_symbol)
        m_active_precedence_directive = true;
    return *this;
}

ggrammar &ggrammar::production(const std::string &identifier, int line) {
    assert(!identifier.empty());
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = non_terminal_symbol(identifier, line);
    // debug begin
    m_log.out << m_log.cggram << "yv::ggram = ";
    m_log.out << "active PRODUCTION ";
    m_log.out << (void *)&*m_active_symbol << " `";
    m_log.out << m_active_symbol->lexeme() << "` ";
    m_log.out << "[uses: " << m_active_symbol.use_count() << "]";
    m_log.out << m_log.creset << "\n";
    // debug end
    return *this;
}

ggrammar &ggrammar::end_production() {
    assert(m_active_symbol);
    // debug begin
    m_log.out << m_log.cggram << "yv::ggram = ";
    m_log.out << "end of PRODUCTION ";
    m_log.out << (void *)&*m_active_symbol << " `";
    m_log.out << m_active_symbol->lexeme() << "` ";
    m_log.out << "[uses: " << m_active_symbol.use_count() << "]";
    m_log.out << m_log.creset << "\n";
    // debug end
    m_associativity = gsymbolassoc::ASSOCIATE_NULL;
    m_active_whitespace_directive = false;
    m_active_precedence_directive = false;
    m_active_production = nullptr;
    m_active_symbol = nullptr;
    return *this;
}

const std::shared_ptr<gsymbol> &ggrammar::non_terminal_symbol(const std::string &lexeme, int line) {
    assert(!lexeme.empty());
    assert(line >= 0);
    return add_symbol(lexeme, line, glexemetype::LEXEME_NULL, gsymboltype::SYMBOL_NON_TERMINAL);
}

const std::shared_ptr<gsymbol> &
ggrammar::add_symbol(const std::string &lexeme, int line, glexemetype lexeme_type, gsymboltype symbol_type) {
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
        // debug begin
        m_log.out << m_log.cggram << "yv::ggram = ";
        m_log.out << "new    PRODUCTION ";
        m_log.out << (void *)&*symbol << " `";
        m_log.out << symbol->lexeme() << "` ";
        m_log.out << "[uses: " << symbol.use_count() << "]";
        m_log.out << m_log.creset << "\n";
        // debug end
        m_symbols.push_back(std::move(symbol));
        // debug begin
        m_log.out << m_log.cggram << "yv::ggram = ";
        m_log.out << "pushed PRODUCTION ";
        m_log.out << (void *)&*m_symbols.back() << " `";
        m_log.out << m_symbols.back()->lexeme() << "` ";
        m_log.out << "[uses: " << m_symbols.back().use_count() << "]";
        m_log.out << m_log.creset << "\n";
        // debug end
        return m_symbols.back();
    }
    assert(i->get());
    assert(i->get()->symbol_type() == symbol_type);
    // debug begin
    m_log.out << m_log.cggram << "yv::ggram = ";
    m_log.out << "found  PRODUCTION ";
    m_log.out << (void *)&*i << " `";
    m_log.out << i->get()->lexeme() << "` ";
    m_log.out << "[uses: " << i->use_count() << "]";
    m_log.out << m_log.creset << "\n";
    // debug end
    return *i;
}
