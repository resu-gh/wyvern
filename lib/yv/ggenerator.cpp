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

#ifndef DEBUG
    /*debug*/ m_log.trace(0) << m_log.op("copy") << "ggram              ->" << m_log.chl << " .m_grammar\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy") << "ggram.identifier   ->" << m_log.chl << " .m_identifier\n";
    /*debug*/ m_log.trace(0) << m_log.op("swap") << "ggram.actions      ->" << m_log.chl << " .m_actions\n";
    /*debug*/ m_log.trace(0) << m_log.op("swap") << "ggram.productions  ->" << m_log.chl << " .m_productions\n";
    /*debug*/ m_log.trace(0) << m_log.op("swap") << "ggram.symbols      ->" << m_log.chl << " .m_symbols\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy") << "ggram.start_symbol ->" << m_log.chl << " .m_start_symbol\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy") << "ggram.end_symbol   ->" << m_log.chl << " .m_end_symbol\n";
    /*debug*/ m_log.trace(0) << m_log.op("copy") << "ggram.error_symbol ->" << m_log.chl << " .m_error_symbol\n";
    /*debug*/ m_log.trace(0) << m_log.op("init") << "m_errors            =" << m_log.chl << " 0\n";
#endif // !DEBUG

    m_errors = 0;

#ifndef DEBUG
    // clang-format off
    /*debug*/ m_log.trace(0) << ".m_identifier " << m_log.chl << m_identifier << "\n";
    /*debug*/ m_log.trace(1) << ".m_errors " << m_log.chl << m_actions.size() << "\n";
    /*debug*/ m_log.trace(1) << ".m_actions\n";
    /*debug*/ for (auto a : m_actions) {
    /*debug*/     m_log.trace(1) << m_log.chl;
    /*debug*/     m_log.out << " *" << reinterpret_cast<std::uintptr_t>(&*a);
    /*debug*/     m_log.out << " <" << a.use_count() << "> ";
    /*debug*/     m_log.out << a->index() << " ";
    /*debug*/     m_log.out << m_log.cwhite << a->identifier() << "\n";
    /*debug*/ }
    /*debug*/ m_log.trace(1) << ".m_productions\n";
    /*debug*/ for (auto p : m_productions) {
    /*debug*/     m_log.trace(1) << m_log.chl;
    /*debug*/     m_log.out << " *" << reinterpret_cast<std::uintptr_t>(&*p);
    /*debug*/     m_log.out << " <" << p.use_count() << "> ";
    /*debug*/     m_log.out << p->index() << " ";
    /*debug*/     m_log.out << m_log.cwhite << p->symbol()->lexeme() << " ";
    /*debug*/     m_log.out << m_log.cnr << "[";
    /*debug*/     m_log.out << m_log.chl;
    /*debug*/     for (auto s : p->symbols()) {
    /*debug*/         m_log.out << s->lexeme();
    /*debug*/         if (&*s != &*(p->symbols().end() - 1)->get())
    /*debug*/             m_log.out << m_log.cnr << "," << m_log.chl;
    /*debug*/     }
    /*debug*/     m_log.out << m_log.cnr;
    /*debug*/     m_log.out << "]\n";
    /*debug*/ }
    /*debug*/ m_log.trace(1) << ".m_symbols\n";
    /*debug*/ for (auto s : m_symbols) {
    /*debug*/     m_log.trace(1) << m_log.chl;
    /*debug*/     m_log.out << " *" << reinterpret_cast<std::uintptr_t>(&*s);
    /*debug*/     m_log.out << " <" << s.use_count() << "> ";
    /*debug*/     m_log.out << s->index() << " ";
    /*debug*/     m_log.out << m_log.cwhite << s->lexeme() << " ";
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
    /*debug*/ m_log.trace(1) << ".m_start_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.chl;
    /*debug*/ m_log.out << " *" << reinterpret_cast<std::uintptr_t>(&*m_start_symbol);
    /*debug*/ m_log.out << " <" << m_start_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_start_symbol->index() << " ";
    /*debug*/ m_log.out << m_log.cwhite << m_start_symbol->lexeme() << " ";
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
    /*debug*/ m_log.trace(1) << ".m_end_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.chl;
    /*debug*/ m_log.out << " *" << reinterpret_cast<std::uintptr_t>(&*m_end_symbol);
    /*debug*/ m_log.out << " <" << m_end_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_end_symbol->index() << " ";
    /*debug*/ m_log.out << m_log.cwhite << m_end_symbol->lexeme() << " ";
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
    /*debug*/ m_log.trace(1) << ".m_error_symbol\n";
    /*debug*/ m_log.trace(1) << m_log.chl;
    /*debug*/ m_log.out << " *" << reinterpret_cast<std::uintptr_t>(&*m_error_symbol);
    /*debug*/ m_log.out << " <" << m_error_symbol.use_count() << "> ";
    /*debug*/ m_log.out << m_error_symbol->index() << " ";
    /*debug*/ m_log.out << m_log.cwhite << m_error_symbol->lexeme() << " ";
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
#endif // !DEBUG

    m_grammar->dump();

    return 0;
}
