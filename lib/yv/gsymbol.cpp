#include "include/gsymbol.hpp"
#include "include/gproduction.hpp"
#include "include/gsymboltype.hpp"

#include <array>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

gsymbol::gsymbol(const std::string &lexeme)
    : m_index(-1),
      m_line(0),
      m_lexeme(lexeme),
      m_identifier(),
      m_symbol_type(gsymboltype::SYMBOL_NULL),
      m_lexeme_type(glexemetype::LEXEME_NULL),
      m_associativity(gsymbolassoc::ASSOCIATE_NULL),
      m_precedence(0),
      m_nullable(false),
      m_productions(),
      m_first(),
      m_follow(),
      m_log("yyv", "gsymb", 255) {}

std::shared_ptr<gsymbol> gsymbol::self() {
    return shared_from_this();
}

int gsymbol::index() const {
    return m_index;
}

int gsymbol::line() const {
    return m_line;
}

const std::string &gsymbol::lexeme() const {
    return m_lexeme;
}

const std::string &gsymbol::identifier() const {
    return m_identifier;
}

gsymboltype gsymbol::symbol_type() const {
    return m_symbol_type;
}

glexemetype gsymbol::lexeme_type() const {
    return m_lexeme_type;
}

gsymbolassoc gsymbol::associativity() const {
    return m_associativity;
}

int gsymbol::precedence() const {
    return m_precedence;
}

bool gsymbol::nullable() const {
    return m_nullable;
}

const std::vector<std::shared_ptr<gproduction>> &gsymbol::productions() const {
    return m_productions;
}

const std::set<std::shared_ptr<gsymbol>, gsymbolc> &gsymbol::first() const {
    return m_first;
}

const std::set<std::shared_ptr<gsymbol>, gsymbolc> &gsymbol::follow() const {
    return m_follow;
}

void gsymbol::set_index(int index) {
    assert(index >= 0);
    m_index = index;
}

void gsymbol::set_line(int line) {
    assert(line >= 0);
    m_line = line;
}

void gsymbol::set_symbol_type(gsymboltype symbol_type) {
    assert(symbol_type >= gsymboltype::SYMBOL_NULL && m_symbol_type < gsymboltype::SYMBOL_TYPE_COUNT);
    m_symbol_type = symbol_type;
}

void gsymbol::set_lexeme_type(glexemetype lexeme_type) {
    assert(lexeme_type >= glexemetype::LEXEME_NULL && lexeme_type < glexemetype::LEXEME_TYPE_COUNT);
    m_lexeme_type = lexeme_type;
}

void gsymbol::set_associativity(gsymbolassoc associativity) {
    assert(associativity >= gsymbolassoc::ASSOCIATE_NONE && associativity <= gsymbolassoc::ASSOCIATE_RIGHT);
    m_associativity = associativity;
}

void gsymbol::set_precedence(int precedence) {
    assert(precedence >= 0);
    m_precedence = precedence;
}

bool gsymbol::matches(const std::string &lexeme, gsymboltype symbol_type) {
    return m_lexeme == lexeme && m_symbol_type == symbol_type;
}

void gsymbol::append_production(const std::shared_ptr<gproduction> &production) {
    assert(production);
    m_productions.push_back(production);
}

void gsymbol::calculate_identifier() {

    assert(!m_lexeme.empty());

    // clang-format off
    std::array<std::string, 128> char_names = {
        "nul", "soh", "stx", "etx", "eot", "enq", "ack", "bel",
        "bs", "tab", "lf", "vt", "ff", "cr", "so", "si",
        "dle", "dc1", "dc2", "dc3", "dc4", "nak", "syn", "etb",
        "can", "em", "sub", "esc", "fs", "gs", "rs", "us",
        "space", "bang", "double_quote", "hash", "dollar", "percent", "amp", "single_quote",
        "left_paren", "right_paren", "star", "plus", "comma", "minus", "dot", "slash",
        "0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "colon", "semi_colon", "lt", "eq", "gt", "question",
        "at", "A", "B", "C", "D", "E", "F", "G",
        "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W",
        "X", "Y", "Z", "left_square_paren", "backslash", "right_square_paren", "hat", "underscore",
        "backtick", "a", "b", "c", "d", "e", "f", "g",
        "h", "i", "j", "k", "l", "m", "n", "o",
        "p", "q", "r", "s", "t", "u", "v", "w",
        "x", "y", "z", "left_curly_brace", "pipe", "right_curly_brace", "tilde", "del",
    };
    // clang-format on

    m_identifier.reserve(m_lexeme.size());
    std::string::const_iterator i = m_lexeme.begin();

    while (i != m_lexeme.end()) {

        if (std::isalnum(*i) || *i == '_') {
            m_identifier.push_back(*i);
            ++i;

        } else {
            if (i != m_lexeme.begin())
                m_identifier.append("_");

            int character = *i;
            assert(character >= 0 && character < 128);
            m_identifier.append(char_names[character]);
            ++i;

            if (i != m_lexeme.end())
                m_identifier.append("_");
        }
    }

    if (m_symbol_type == gsymboltype::SYMBOL_TERMINAL)
        m_identifier.append("_terminal");
}

std::shared_ptr<gsymbol> gsymbol::implicit_terminal() const {
    std::shared_ptr<gsymbol> implicit_terminal = nullptr;

    if (m_productions.size() == 1) {

        std::shared_ptr<gproduction> production = m_productions.front(); // maybe weak_ptr
        assert(production.get());

        if (production.get()->length() == 1 && !production->action().get()) {

            std::shared_ptr<gsymbol> symbol = production->symbols().front();

            if (symbol->symbol_type() == gsymboltype::SYMBOL_TERMINAL)
                implicit_terminal = symbol;
        }
    }

    return implicit_terminal;
}

void gsymbol::replace_by_non_terminal(const std::shared_ptr<gsymbol> &non_terminal_symbol) {
    assert(m_symbol_type == gsymboltype::SYMBOL_TERMINAL);
    assert(non_terminal_symbol.get());
    assert(non_terminal_symbol->m_symbol_type == gsymboltype::SYMBOL_NON_TERMINAL);

    m_identifier = non_terminal_symbol->m_lexeme;
    m_precedence = non_terminal_symbol->m_precedence;
    m_associativity = non_terminal_symbol->m_associativity;
}

/// calculate the first set and nullable for this symbol
/// returns the number of symbols in the first set
/// + 1 if nullable changed for this symbol
int gsymbol::calculate_first() {
    int added = 0;

    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
    using prod_symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;

    if (m_symbol_type == gsymboltype::SYMBOL_NON_TERMINAL) {

        for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {

            std::shared_ptr<gproduction> production = *i;
            assert(production.get());

            const std::vector<std::shared_ptr<gsymbol>> &symbols = production->symbols();

            prod_symb_iter j = symbols.begin();
            while (j != symbols.end() && j->get()->nullable()) {

                std::shared_ptr<gsymbol> symbol = *j;
                assert(symbol.get());

                added += add_symbols_to_first(symbol->first());

                ++j;
            }

            if (j != symbols.end()) {

                std::shared_ptr<gsymbol> symbol = *j;
                assert(symbol.get());

                added += add_symbols_to_first(symbol->first());

            } else {

                added += m_nullable ? 0 : 1;
                m_nullable = true;
            }
        }

    } else {

        added += add_symbol_to_first(self());
    }

    return added;
}

/// add 1 symbol to the first set of this symbol
int gsymbol::add_symbol_to_first(const std::shared_ptr<gsymbol> &symbol) {
    assert(symbol.get());
    return m_first.insert(symbol).second ? 1 : 0;
}

/// add 1 or more symbols to the first set of this symbol
int gsymbol::add_symbols_to_first(const std::set<std::shared_ptr<gsymbol>, gsymbolc> &symbols) {
    std::size_t original_size = m_first.size();
    m_first.insert(symbols.begin(), symbols.end());
    return int(m_first.size() - original_size);
}

/// calculate the follow set for this symbol
int gsymbol::calculate_follow() {
    int added = 0;

    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;

    for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {

        std::shared_ptr<gproduction> production = *i; // TODO maybe weak_ptr or const &
        assert(production.get());

        const std::vector<std::shared_ptr<gsymbol>> &symbols = production->symbols();

        if (!symbols.empty()) {

            using symb_riter = std::vector<std::shared_ptr<gsymbol>>::const_reverse_iterator;
            symb_riter j = symbols.rbegin();

            std::shared_ptr<gsymbol> symbol = *j;

            added += symbol->add_symbols_to_follow(follow());
            ++j;

            while (j != symbols.rend() && symbol->nullable()) {

                std::shared_ptr<gsymbol> previous_symbol = *j;
                assert(previous_symbol.get());

                added += previous_symbol->add_symbols_to_follow(follow());
                symbol = previous_symbol;

                ++j;
            }

            while (j != symbols.rend()) {

                std::shared_ptr<gsymbol> previous_symbol = *j;
                assert(previous_symbol.get());

                added += previous_symbol->add_symbols_to_follow(symbol->first());
                ++j;
            }
        }
    }

    return added;
}

/// add 1 symbol to the follow set of this symbol
int gsymbol::add_symbol_to_follow(const std::shared_ptr<gsymbol> &symbol) {
    assert(symbol.get());
    return m_follow.insert(symbol).second ? 1 : 0;
}

/// add 1 or more symbols to the follow set of this symbol
int gsymbol::add_symbols_to_follow(const std::set<std::shared_ptr<gsymbol>, gsymbolc> &symbols) {
    std::size_t original_size = m_follow.size();
    m_follow.insert(symbols.begin(), symbols.end());
    return int(m_follow.size() - original_size);
}

std::string gsymbol::microdump() const {
    std::stringstream s;
    s << (void *)&*this << " ";
    s << m_index << " ";
    s << m_identifier << " ";
    s << m_lexeme << " ";
    return s.str();
}

void gsymbol::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "gsymb";
    m_log.out << m_log.cnr << (inlined ? ": { " : ": {\n");

    if (uc) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "use_count: ";
        m_log.out << m_log.chl << uc;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "this: ";
    m_log.out << m_log.chl << &*this;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "index: ";
    m_log.out << m_log.chl << m_index;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    if (m_identifier.empty()) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "lexeme: ";
        m_log.out << m_log.chl << m_lexeme;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    } else {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "identifier: ";
        m_log.out << m_log.chl << m_identifier;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    if (!inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "lexeme" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_lexeme << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "line" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_line << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "symbol_type" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_symbol_type << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "lexeme_type" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_lexeme_type << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "associativity" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_associativity << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "precedence" << m_log.cnr << ": ";
        m_log.out << m_log.chl << m_precedence << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "nullable" << m_log.cnr << ": ";
        m_log.out << m_log.chl << (m_nullable ? "true" : "false") << m_log.cnr << ",\n";
    }

    // recursive begin
    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "productions: [";
        m_log.out << (m_productions.size() ? "\n" : "");
        for (auto p : m_productions)
            p->json(sc + 4, true, sc + 4, true, p.use_count());
        m_log.out << m_log.cnr << m_log.sp(m_productions.size() ? sc + 2 : 0) << "],\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "first: [";
        m_log.out << (m_first.size() ? "\n" : "");
        for (auto f : m_first)
            f->json(sc + 4, true, sc + 4, true, f.use_count());
        m_log.out << m_log.cnr << m_log.sp(m_first.size() ? sc + 2 : 0) << "],\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "follow: [";
        m_log.out << (m_follow.size() ? "\n" : "");
        for (auto f : m_follow)
            f->json(sc + 4, true, sc + 4, true, f.use_count());
        m_log.out << m_log.cnr << m_log.sp(m_follow.size() ? sc + 2 : 0) << "],\n";
    }
    // recursive begin

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
