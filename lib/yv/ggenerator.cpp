#include "include/ggenerator.hpp"
#include "include/ecode.hpp"
#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"

#include <cassert>
#include <cstdint>
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

    // clang-format off
    /*debug*/ m_log.trace(0) << m_log.op("dump") << ".m_identifier ";
    /*debug*/ m_log.out << m_log.chl << m_identifier << "\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_errors ";
    /*debug*/ m_log.out << m_log.chl << m_errors << "\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_actions\n";
    /*debug*/ for (auto a : m_actions) {
    /*debug*/     m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/     m_log.out << "<" << a.use_count() << "> ";
    /*debug*/     m_log.out << m_log.cwhite << a->microdump() << "\n";
    /*debug*/ }
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_productions\n";
    /*debug*/ for (auto p : m_productions) {
    /*debug*/     m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/     m_log.out << "<" << p.use_count() << "> ";
    /*debug*/     m_log.out << m_log.cwhite << p->microdump();
    /*debug*/     m_log.out << m_log.chl << "on " << m_log.cwhite;
    /*debug*/     m_log.out << p->symbol()->lexeme() << " ";
    /*debug*/     m_log.out << m_log.cnr << "to [";
    /*debug*/     m_log.out << m_log.chl;
    /*debug*/     for (auto s : p->symbols()) {
    /*debug*/         m_log.out << s->lexeme();
    /*debug*/         if (&*s != &*(p->symbols().end() - 1)->get())
    /*debug*/             m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/     }
    /*debug*/     m_log.out << m_log.cnr;
    /*debug*/     m_log.out << "]" << m_log.chl;
    /*debug*/     if (p->action()) {
    /*debug*/         m_log.out << " w/ " << m_log.cwhite;
    /*debug*/         m_log.out << p->action()->identifier();
    /*debug*/     }
    /*debug*/     m_log.out << "\n";
    /*debug*/ }
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_symbols\n";
    /*debug*/ for (auto s : m_symbols) {
    /*debug*/     m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/     m_log.out << " <" << s.use_count() << "> ";
    /*debug*/     m_log.out << m_log.cwhite << s->microdump();
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
    /*debug*/     m_log.out << m_log.cnr;
    /*debug*/     m_log.out << "\n";
    /*debug*/ }
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_start_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << " <" << m_start_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_log.cwhite << m_start_symbol->microdump();
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
    /*debug*/ m_log.out << m_log.cnr;
    /*debug*/ m_log.out << "\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_end_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << " <" << m_end_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_log.cwhite << m_end_symbol->microdump();
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
    /*debug*/ m_log.trace(1) << m_log.op("") << ".m_error_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.op("") << m_log.chl;
    /*debug*/ m_log.out << " <" << m_error_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_log.cwhite << m_error_symbol->microdump();
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
    /*debug*/ m_log.out << m_log.cnr;
    /*debug*/ m_log.out << "\n";
    // clang-format on

    // m_grammar->dump();

    calculate_identifiers();
    check_for_undefined_symbol_errors();
    check_for_unreferenced_symbol_errors();
    // check_for_error_symbol_on_left_hand_side_errors();

    if (m_errors == 0) {
        // calculate_terminal_and_non_terminal_symbols();
        // calculate_implicit_terminal_symbols();
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
    /*debug*/ m_log.set_fun("calc_idents");

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
        /*debug*/ m_log.set_fun("calc_idents");
        /*debug*/ m_log.trace(1) << m_log.op("proc") << "symbol: ";
        /*debug*/ m_log.out << m_log.chl << i->get()->lexeme() << "\n";

        assert(i->get());
        /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
        /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

        i->get()->calculate_identifier();
        /*debug*/ m_log.set_fun("calc_idents");
        /*debug*/ m_log.trace(0) << m_log.op("get") << "(symbol).iden ";
        /*debug*/ m_log.out << m_log.chl << i->get()->identifier() << "\n";
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
            /*debug*/ m_log.trace(1) << m_log.op("proc") << "symbol: ";
            /*debug*/ m_log.out << m_log.chl << i->get()->lexeme() << "\n";

            assert(i->get());
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
            /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

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
            /*debug*/ m_log.trace(1) << m_log.op("proc") << "symbol: ";
            /*debug*/ m_log.out << m_log.chl << i->get()->lexeme() << "\n";

            assert(i->get());
            /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
            /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

            int references = 0;
            std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr

            if (symbol != m_start_symbol && symbol != m_end_symbol && symbol->lexeme() != m_error_symbol->lexeme()) {

                using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
                for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {
                    /*debug*/ m_log.trace(1) << m_log.op("proc") << "production: ";
                    /*debug*/ m_log.out << m_log.chl << i->get()->index() << "\n";

                    assert(i->get());
                    /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite << "OK ";
                    /*debug*/ m_log.out << m_log.chl << &*i->get() << m_log.cnr << " != nullptr\n";

                    std::shared_ptr<gproduction> production = *i;

                    if (production->symbol()->symbol_type() != gsymboltype::SYMBOL_NON_TERMINAL) {
                        ;
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
