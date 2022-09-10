#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"

#include <cstdint>
#include <iostream>

ggenerator::ggenerator()
    : m_identifier(),
      m_actions(),
      m_productions(),
      m_symbols(),
      m_errors(0),
      m_log("ggenr", 40) {}

int ggenerator::generate(const std::shared_ptr<ggrammar> &grammar) {
    m_log.set_function("generate");

    m_grammar = grammar;
    m_identifier = m_grammar->identifier();
    m_actions.swap(m_grammar->actions());
    m_productions.swap(m_grammar->productions());
    m_symbols.swap(m_grammar->symbols());
    m_start_symbol = m_grammar->start_symbol();
    m_end_symbol = m_grammar->end_symbol();
    m_error_symbol = m_grammar->error_symbol();

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

    m_errors = 0;

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

    // calculate_identifiers();
    // check_for_undefined_symbol_errors();
    // check_for_unreferenced_symbol_errors();
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
