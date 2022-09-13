#include "include/ggenerator.hpp"
#include "include/ecode.hpp"
#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>

ggenerator::ggenerator()
    : m_identifier(),
      m_actions(),
      m_productions(),
      m_symbols(),
      m_errors(0),
      m_log("yyv", "ggenr", 40) {}

int ggenerator::generate(const std::shared_ptr<ggrammar> &grammar) {
    m_log.set_fun("generate");

    // m_grammar->dump();

    m_grammar = grammar;
    m_identifier = m_grammar->identifier();       // if useless remove grammar shared ref
    m_actions.swap(m_grammar->actions());         // ownership moved
    m_productions.swap(m_grammar->productions()); // ownership moved
    m_symbols.swap(m_grammar->symbols());         // ownership moved
    m_start_symbol = m_grammar->start_symbol();   // if useless remove grammar shared ref
    m_end_symbol = m_grammar->end_symbol();       // if useless remove grammar shared ref
    m_error_symbol = m_grammar->error_symbol();   // if useless remove grammar shared ref
    m_errors = 0;                                 // init error

    /*debug*/ m_log.trace(0) << m_log.op("copy");
    /*debug*/ m_log.out << "grammar              -> " << m_log.chl << ".m_grammar\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy");
    /*debug*/ m_log.out << "grammar.identifier   -> " << m_log.chl << ".m_identifier\n";
    /*debug*/ m_log.trace(0) << m_log.op("move");
    /*debug*/ m_log.out << "grammar.actions      -> " << m_log.chl << ".m_actions\n";
    /*debug*/ m_log.trace(0) << m_log.op("move");
    /*debug*/ m_log.out << "grammar.productions  -> " << m_log.chl << ".m_productions\n";
    /*debug*/ m_log.trace(0) << m_log.op("move");
    /*debug*/ m_log.out << "grammar.symbols      -> " << m_log.chl << ".m_symbols\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy");
    /*debug*/ m_log.out << "grammar.start_symbol -> " << m_log.chl << ".m_start_symbol\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy");
    /*debug*/ m_log.out << "grammar.end_symbol   -> " << m_log.chl << ".m_end_symbol\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy");
    /*debug*/ m_log.out << "grammar.error_symbol -> " << m_log.chl << ".m_error_symbol\n";
    /*debug*/ m_log.trace(0) << m_log.op("init");
    /*debug*/ m_log.out << "m_errors              = " << m_log.chl << "0\n";

    // dump();
    // m_grammar->dump();

    calculate_identifiers();
    check_for_undefined_symbol_errors();
    check_for_unreferenced_symbol_errors();
    check_for_error_symbol_on_left_hand_side_errors();

    if (m_errors == 0) {
        calculate_terminal_and_non_terminal_symbols(); // FIXME or fixed? (modified)
        dump();
        calculate_implicit_terminal_symbols();
        dump();
        // calculate_symbol_indices();
        // calculate_first();
        // calculate_follow();
        // calculate_precedence_of_productions();
        // generate_states( m_start_symbol, m_end_symbol, m_symbols );
    }

    // TODO why this strange swap
    int errors = m_errors;
    m_errors = 0;

    return errors;
}

/// calculate identifiers for all symbols
void ggenerator::calculate_identifiers() {
    /*debug*/ m_log.set_fun("cmp_idents");

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        /*debug*/ m_log.set_fun("cmp_idents");
        /*debug*/ m_log.trace(1) << m_log.op("proc");
        /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
        /*debug*/ m_log.out << i->get()->microdump() << "\n";

        assert(i->get());

        i->get()->calculate_identifier();
        /*debug*/ m_log.set_fun("cmp_idents");
        /*debug*/ m_log.trace(0) << m_log.op("get");
        /*debug*/ m_log.out << m_log.cwhite << i->get()->identifier() << "\n";
    }
}

/// check for symbols in the grammar that
/// are referenced but never defined
void ggenerator::check_for_undefined_symbol_errors() {
    /*debug*/ m_log.set_fun("ck_undef_sym");

    if (m_errors == 0) {
        /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";

        using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
            /*debug*/ m_log.trace(1) << m_log.op("proc");
            /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
            /*debug*/ m_log.out << i->get()->microdump() << "\n";

            assert(i->get());

            std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr

            if (symbol->symbol_type() == gsymboltype::SYMBOL_NON_TERMINAL && symbol->productions().empty() && symbol->precedence() <= 0) {
                ++m_errors;
                /*error*/ m_log.etrace(0) << m_log.op("error");
                /*error*/ m_log.err << m_log.cred << ecode::E_PARSER_UNDEF_SYMBOL << " ";
                /*error*/ m_log.err << "(line " << symbol->line() << ")\n";
                /*error*/ m_log.etrace(1) << m_log.op("") << m_log.cred << "undefined symbol ";
                /*error*/ m_log.err << m_log.cwhite << symbol->identifier() << "\n";
            }
        }
    }
}

/// check for symbols in the grammar that are defined but never referenced
void ggenerator::check_for_unreferenced_symbol_errors() {
    /*debug*/ m_log.set_fun("ck_unref_sym");

    if (m_errors == 0) {
        /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";

        using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
            /*debug*/ m_log.trace(0) << m_log.op("symbol");
            /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
            /*debug*/ m_log.out << i->get()->microdump() << "\n";

            assert(i->get());
            // /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
            // /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

            int references = 0;
            std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr

            if (symbol != m_start_symbol && symbol != m_end_symbol && symbol->lexeme() != m_error_symbol->lexeme()) {

                /*debug*/ m_log.trace(1) << m_log.op("iter") << m_log.chl << ".m_productions\n";
                using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
                for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {
                    /*debug*/ m_log.trace(1) << m_log.op("produc");
                    /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
                    /*debug*/ m_log.out << i->get()->microdump() << "\n";

                    assert(i->get());
                    // /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
                    // /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

                    std::shared_ptr<gproduction> production = *i; // maybe weak_ptr

                    if (production->symbol()->symbol_type() != gsymboltype::SYMBOL_TERMINAL) {
                        references += production->count_references_to_symbol(symbol);
                    }
                }

                if (references == 0) {
                    ++m_errors;
                    /*error*/ m_log.etrace(0) << m_log.op("error");
                    /*error*/ m_log.err << m_log.cred << ecode::E_PARSER_UNREF_SYMBOL << " ";
                    /*error*/ m_log.err << "(line " << symbol->line() << ")\n";
                    /*error*/ m_log.etrace(1) << m_log.op("") << m_log.cred << "unreferenced symbol ";
                    /*error*/ m_log.err << m_log.cwhite << symbol->identifier() << "\n";
                }
            }
        }
    }
}

/// check for the error symbol being
/// used in the lhs of a production
void ggenerator::check_for_error_symbol_on_left_hand_side_errors() {
    /*debug*/ m_log.set_fun("ck_error_lhs");

    assert(m_error_symbol.get());

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        /*debug*/ m_log.trace(1) << m_log.op("symbol");
        /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
        /*debug*/ m_log.out << i->get()->microdump() << "\n";

        assert(i->get());
        // /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
        // /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

        std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr

        if (!symbol->productions().empty() && symbol->lexeme() == m_error_symbol->lexeme()) {
            ++m_errors;
            /*error*/ m_log.etrace(0) << m_log.op("error");
            /*error*/ m_log.err << m_log.cred << ecode::E_PARSER_ERROR_ON_LHS << " ";
            /*error*/ m_log.err << "(line " << symbol->line() << ")\n";
            /*error*/ m_log.etrace(1) << m_log.op("") << m_log.cred;
            /*error*/ m_log.err << "the `error` symbol appears on the lhs of this production\n";
        }
    }
}

/// calculate which symbols are terminal and non-terminal
/// any symbol w/ 1 or more productions are assumed to be non-terminal
/// any symbol w/ no productions are assumed to be terminal
/// (.start, .end and .error are not processed)
void ggenerator::calculate_terminal_and_non_terminal_symbols() {
    /*debug*/ m_log.set_fun("cmp_t_nt");

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        /*debug*/ m_log.trace(1) << m_log.op("symbol");
        /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
        /*debug*/ m_log.out << i->get()->microdump() << "\n";

        assert(i->get());
        // /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
        // /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

        std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr

        /// i need to skip over .start, .end and .error (description)
        /// TODO added
        if (symbol == m_start_symbol || symbol == m_end_symbol || symbol == m_error_symbol)
            continue;
        /// TODO added

        /*debug*/ m_log.trace(1) << m_log.op("");
        /*debug*/ m_log.out << i->get()->symbol_type() << " -> ";
        /// FIXME very strange check
        if (symbol->symbol_type() == gsymboltype::SYMBOL_NULL) {
            if (symbol->productions().empty()) {
                symbol->set_symbol_type(gsymboltype::SYMBOL_TERMINAL);
                /*debug*/ m_log.out << m_log.cyellow << i->get()->symbol_type() << "\n";
            } else {
                symbol->set_symbol_type(gsymboltype::SYMBOL_NON_TERMINAL);
                /*debug*/ m_log.out << m_log.cyellow << i->get()->symbol_type() << "\n";
            }
        }
        // clang-format off
        /*debug*/ else {
        /*debug*/ m_log.out << m_log.cnr << i->get()->symbol_type() << "\n";
        /*debug*/ }
        // clang-format on
    }
}

/// calculate non-terminal symbols that are really just named terminals
/// any symbol that contain a single production that contains only a terminal
/// symbol are really just acting as names for that terminal symbol
/// ---
/// to make the parser easier to understand and more efficient these symbols
/// are collapsed by making any references to the non-terminal symbol refer
/// directly to the terminal symbol
/// ---
/// example: 'integer: "[0-9]+'
/// - non-terminal `integer`
/// - terminal `[0-9]+`
/// - trivial reduction from the point of view of the parser
/// =
/// the terminal is collapsed into the non-terminal keeping the
/// more readable name of the non-terminal but removing the
/// redundant reduction
///
/// TODO FIXME:
/// .start, .end, .error must be processed?
/// can i skip all that is not non-terminal?
void ggenerator::calculate_implicit_terminal_symbols() {
    /*debug*/ m_log.set_fun("cmp_impl_t");

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator; // FIXME iter or const_iter ?
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        // /*debug*/ m_log.trace(1) << m_log.op("symbol");
        // /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
        // /*debug*/ m_log.out << i->get()->microdump() << "\n";

        std::shared_ptr<gsymbol> non_terminal_symbol = *i; // maybe weak_ptr

        if (non_terminal_symbol.get() && non_terminal_symbol != m_error_symbol) {
            /*debug*/ m_log.trace(1) << m_log.op("noterm");
            /*debug*/ m_log.out << m_log.chl << "<" << i->use_count() << "> ";
            /*debug*/ m_log.out << i->get()->microdump() << i->get()->symbol_type() << "\n";

            std::shared_ptr<gsymbol> terminal_symbol = non_terminal_symbol->implicit_terminal();
            if (terminal_symbol) {
                /*debug*/ m_log.trace(1) << m_log.op("impli");
                /*debug*/ m_log.out << m_log.cwhite << "<" << terminal_symbol.use_count() << "> ";
                /*debug*/ m_log.out << terminal_symbol.get()->microdump() << "\n";

                assert(terminal_symbol.get() != non_terminal_symbol.get());
                terminal_symbol->replace_by_non_terminal(non_terminal_symbol);
                replace_references_to_symbol(non_terminal_symbol, terminal_symbol);

                /*debug*/ m_log.trace(1) << m_log.op("reset") << m_log.cred << "<";
                /*debug*/ m_log.out << i->use_count() << "> " << &*i << " " << i->get() << "\n";
                // /*debug*/ m_log.out << i->get()->microdump() << "\n";
                i->reset();
            }
        }
    }

    symb_iter i = m_symbols.begin();
    while (i != m_symbols.end())
        if (!i->get()) { // i->get() == nullptr
            /*debug*/ m_log.trace(1) << m_log.op("erase") << m_log.cred << "<";
            /*debug*/ m_log.out << i->use_count() << "> " << &*i << " " << i->get() << "\n";
            i = m_symbols.erase(i);
        } else {
            /*debug*/ m_log.trace(1) << m_log.op("symbol");
            /*debug*/ m_log.out << i->get()->microdump() << m_log.cwhite;
            /*debug*/ m_log.out << i->get()->identifier() << "\n";
            ++i;
        }

    // i = (!i->get()) ? m_symbols.erase(i) : i + 1;
    // TODO FIXME check why all gsymbols have the same index
}

void ggenerator::replace_references_to_symbol(const std::shared_ptr<gsymbol> &to_symbol, const std::shared_ptr<gsymbol> &with_symbol) {
    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
    for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {

        std::shared_ptr<gproduction> production = *i;
        assert(production.get());
        production->replace_references_to_symbol(to_symbol, with_symbol);
    }
}

void ggenerator::dump() const {
    // clang-format off

    /*debug*/ m_log.trace(0) << m_log.op("dump") << "\n";
    /*debug*/ m_log.out << m_log.op("") << ".m_identifier ";
    /*debug*/ m_log.out << m_log.chl << m_identifier << "\n";

    /*debug*/ m_log.out << m_log.op("") << ".m_errors ";
    /*debug*/ m_log.out << m_log.chl << m_errors << "\n";

    /*debug*/ m_log.out << m_log.op("") << ".m_actions\n";
    /*debug*/ for (auto a : m_actions) {
    /*debug*/     m_log.out << m_log.op("") << m_log.chl;
    /*debug*/     m_log.out << "<" << a.use_count() << "> ";
    /*debug*/     m_log.out << m_log.chl << &*a << " ";
    /*debug*/     m_log.out << a->index() << " ";
    /*debug*/     m_log.out << m_log.cwhite << a->identifier() << "\n";
    /*debug*/ }

    /*debug*/ m_log.out << m_log.op("") << ".m_productions\n";
    /*debug*/ for (auto p : m_productions) {
    /*debug*/     m_log.out << m_log.op("") << m_log.chl;
    /*debug*/     m_log.out << "<" << p.use_count() << "> ";
    /*debug*/     m_log.out << m_log.chl << &*p << " ";
    /*debug*/     m_log.out << m_log.cwhite << p->index() << " ";
    /*debug*/     m_log.out << m_log.cnr << "on " << m_log.cwhite;
    /*debug*/     if (!p->symbol()->identifier().empty())
    /*debug*/         m_log.out << p->symbol()->identifier() << " ";
    /*debug*/     else
    /*debug*/         m_log.out << "`"<< p->symbol()->lexeme() << "` ";
    /*debug*/     m_log.out << m_log.cnr << "to [";
    /*debug*/     m_log.out << m_log.chl;
    /*debug*/     for (auto s : p->symbols()) {
    /*debug*/         if (!s->identifier().empty())
    /*debug*/             m_log.out << s->identifier();
    /*debug*/         else
    /*debug*/             m_log.out << "`" << s->lexeme() << "`";
    /*debug*/         if (&*s != &*(p->symbols().end() - 1)->get())
    /*debug*/             m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/     }
    /*debug*/     m_log.out << m_log.cnr;
    /*debug*/     m_log.out << "]" << m_log.cnr;
    /*debug*/     if (p->action()) {
    /*debug*/         m_log.out << " w/ " << m_log.cwhite;
    /*debug*/         m_log.out << p->action()->identifier();
    /*debug*/     }
    /*debug*/     m_log.out << "\n";
    /*debug*/ }

    /*debug*/ m_log.out << m_log.op("") << ".m_symbols\n";
    /*debug*/ for (auto s : m_symbols) {
    /*debug*/     m_log.out << m_log.op("") << m_log.chl;
    /*debug*/     m_log.out << " <" << s.use_count() << "> ";
    /*debug*/     m_log.out << m_log.chl << &*s << " ";
    /*debug*/     m_log.out << m_log.cwhite << s->index() << " ";
    /*debug*/     if (!s->identifier().empty())
    /*debug*/         m_log.out << m_log.cwhite << s->identifier() << " ";
    /*debug*/     else
    /*debug*/         m_log.out << "undef ";
    /*debug*/     m_log.out << m_log.chl << s->lexeme() << " ";
    /*debug*/     m_log.out << m_log.cnr << s->precedence() << " ";
    /*debug*/     m_log.out << s->symbol_type() << " ";
    /*debug*/     m_log.out << s->lexeme_type() << " ";
    /*debug*/     m_log.out << s->associativity() << " ";
    /*debug*/     if (s->productions().size()) {
    /*debug*/         m_log.out << "[" << m_log.chl;
    /*debug*/         for (auto p : s->productions()) {
    /*debug*/             m_log.out << p->index();
    /*debug*/             if (&*p != &*(s->productions().end() - 1)->get())
    /*debug*/                 m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/         }
    /*debug*/         m_log.out << m_log.cnr << "]";
    /*debug*/     }
    /*debug*/     m_log.out << "\n";
    /*debug*/ }

    /*debug*/ m_log.out << m_log.op("") << ".m_start_symbol\n";
    /*debug*/ m_log.out << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << " <" << m_start_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_log.chl << &*m_start_symbol << " ";
    /*debug*/ m_log.out << m_log.cwhite << m_start_symbol->index() << " ";
    /*debug*/ if (!m_start_symbol->identifier().empty())
    /*debug*/     m_log.out << m_log.cwhite << m_start_symbol->identifier() << " ";
    /*debug*/ else
    /*debug*/     m_log.out << "undef ";
    /*debug*/ m_log.out << m_log.chl << m_start_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cnr << m_start_symbol->precedence() << " ";
    /*debug*/ m_log.out << m_start_symbol->symbol_type() << " ";
    /*debug*/ m_log.out << m_start_symbol->lexeme_type() << " ";
    /*debug*/ m_log.out << m_start_symbol->associativity() << " ";
    /*debug*/ if (m_start_symbol->productions().size()) {
    /*debug*/     m_log.out << "[" << m_log.chl;
    /*debug*/     for (auto p : m_start_symbol->productions()) {
    /*debug*/         m_log.out << p->index();
    /*debug*/         if (&*p != &*(m_start_symbol->productions().end() - 1)->get())
    /*debug*/             m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/     }
    /*debug*/     m_log.out << m_log.cnr << "]";
    /*debug*/ }
    /*debug*/ m_log.out << "\n";

    /*debug*/ m_log.out << m_log.op("") << ".m_end_symbol\n";
    /*debug*/ m_log.out << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << " <" << m_end_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_log.chl << &*m_end_symbol << " ";
    /*debug*/ m_log.out << m_log.cwhite << m_end_symbol->index() << " ";
    /*debug*/ if (!m_end_symbol->identifier().empty())
    /*debug*/     m_log.out << m_log.cwhite << m_end_symbol->identifier() << " ";
    /*debug*/ else
    /*debug*/     m_log.out << "undef ";
    /*debug*/ m_log.out << m_log.chl << m_end_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cnr << m_end_symbol->precedence() << " ";
    /*debug*/ m_log.out << m_end_symbol->symbol_type() << " ";
    /*debug*/ m_log.out << m_end_symbol->lexeme_type() << " ";
    /*debug*/ m_log.out << m_end_symbol->associativity() << " ";
    /*debug*/ if (m_end_symbol->productions().size()) {
    /*debug*/     m_log.out << "[" << m_log.chl;
    /*debug*/     for (auto p : m_end_symbol->productions()) {
    /*debug*/         m_log.out << p->index();
    /*debug*/         if (&*p != &*(m_end_symbol->productions().end() - 1)->get())
    /*debug*/             m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/     }
    /*debug*/     m_log.out << m_log.cnr << "]";
    /*debug*/ }
    /*debug*/ m_log.out << m_log.cnr;
    /*debug*/ m_log.out << "\n";

    /*debug*/ m_log.out << m_log.op("") << ".m_error_symbol\n";
    /*debug*/ m_log.out << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << " <" << m_error_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_log.chl << &*m_error_symbol << " ";
    /*debug*/ m_log.out << m_log.cwhite << m_error_symbol->index() << " ";
    /*debug*/ if (!m_error_symbol->identifier().empty())
    /*debug*/     m_log.out << m_log.cwhite << m_error_symbol->identifier() << " ";
    /*debug*/ else
    /*debug*/     m_log.out << "undef ";
    /*debug*/ m_log.out << m_log.chl << m_error_symbol->lexeme() << " ";
    /*debug*/ m_log.out << m_log.cnr << m_error_symbol->precedence() << " ";
    /*debug*/ m_log.out << m_error_symbol->symbol_type() << " ";
    /*debug*/ m_log.out << m_error_symbol->lexeme_type() << " ";
    /*debug*/ m_log.out << m_error_symbol->associativity() << " ";
    /*debug*/ if (m_error_symbol->productions().size()) {
    /*debug*/     m_log.out << "[" << m_log.chl;
    /*debug*/     for (auto p : m_error_symbol->productions()) {
    /*debug*/         m_log.out << p->index();
    /*debug*/         if (&*p != &*(m_error_symbol->productions().end() - 1)->get())
    /*debug*/             m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/     }
    /*debug*/     m_log.out << m_log.cnr << "]";
    /*debug*/ }
    /*debug*/ m_log.out << "\n";
    // clang-format on
}
