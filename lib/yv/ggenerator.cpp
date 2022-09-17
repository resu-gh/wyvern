#include "include/ggenerator.hpp"
#include "include/ecode.hpp"
#include "include/ggrammar.hpp"
#include "include/gproduction.hpp"
#include "include/gstate.hpp"
#include "include/gtransition.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

ggenerator::ggenerator()
    : m_identifier(),
      m_actions(),
      m_productions(),
      m_symbols(),
      m_start_symbol(nullptr),
      m_end_symbol(nullptr),
      m_error_symbol(nullptr),
      m_states(),
      m_start_state(nullptr),
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
    m_states.clear();                             // TODO maybe can be avoided

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
        calculate_implicit_terminal_symbols();
        calculate_symbol_indices();
        calculate_first();
        calculate_follow();
        calculate_precedence_of_productions();
        // generate_states( m_start_symbol, m_end_symbol, m_symbols );
        generate_states();
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
    /*debug*/ m_log.set_fun("repl_symb_refs");

    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
    for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {

        std::shared_ptr<gproduction> production = *i;
        assert(production.get());
        production->replace_references_to_symbol(to_symbol, with_symbol);
    }
}

/// calculate the index for each symbol
void ggenerator::calculate_symbol_indices() {
    /*debug*/ m_log.set_fun("cmp_indices");

    int index = 0;
    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator; // FIXME iter or const_iter ?
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

        std::shared_ptr<gsymbol> symbol = *i;
        assert(i->get());

        symbol->set_index(index);
        /*debug*/ m_log.trace(1) << m_log.op("symbol");
        /*debug*/ m_log.out << m_log.cwhite << symbol->index() << " ";
        /*debug*/ m_log.out << m_log.chl << symbol->identifier() << "\n";
        ++index;
    }
}

/// calculate the first position sets for each symbol
/// until no more terminals can be added to any first
/// position sets
void ggenerator::calculate_first() {
    /*debug*/ m_log.set_fun("cmp_first");

    int added = 1;
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator; // FIXME iter or const_iter ?
                                                                       /// FIXME this is a do-while
    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << "until no changes in the first sets\n";
    while (added > 0) {
        added = 0;
        /*debug*/ m_log.trace(1) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
            std::shared_ptr<gsymbol> symbol = *i;
            assert(i->get());
            /*debug*/ m_log.trace(1) << m_log.op("symbol") << m_log.chl;
            /*debug*/ m_log.out << symbol->microdump() << "\n";

            added += symbol->calculate_first();
            /*debug*/ m_log.trace(0) << m_log.op("first") << m_log.chl << symbol->microdump() << "\n";
            // clang-format off
            /*debug*/ for (auto s : symbol->first())
            /*debug*/     m_log.trace(1) << m_log.op("") << m_log.cwhite << s->microdump() << "\n";
            // clang-format on
        }
    }
}

/// calculate the follow positions sets for each symbol
/// until no more terminals can be added to any follow
/// position sets
void ggenerator::calculate_follow() {
    /*debug*/ m_log.set_fun("cmp_follow");

    m_start_symbol->add_symbol_to_follow(m_end_symbol); // TODO maybe useless (repeated inside the loop)

    int added = 1;
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator;

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << "until no changes in the follow sets\n";
    while (added > 0) {
        added = 0;
        /*debug*/ m_log.trace(1) << m_log.op("iter") << m_log.chl << ".m_symbols\n";
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
            std::shared_ptr<gsymbol> symbol = *i;
            assert(symbol.get());
            /*debug*/ m_log.trace(0) << m_log.op("symbol") << m_log.chl;
            /*debug*/ m_log.out << symbol->microdump() << "\n";

            added += symbol->calculate_follow();
            /*debug*/ m_log.trace(0) << m_log.op("follow") << m_log.chl << symbol->microdump() << "\n";
            // clang-format off
            /*debug*/ for (auto s : symbol->follow())
            /*debug*/     m_log.trace(1) << m_log.op("") << m_log.cwhite << s->microdump() << "\n";
            // clang-format on
        }
        /*debug*/ m_log.trace(1) << m_log.op("get") << "added: " << m_log.chl << added << "\n";
    }
}

/// calculate the precedence of each production that
/// hasn't had precedence set explicitly
/// as the precedence of its rightmost terminal
void ggenerator::calculate_precedence_of_productions() {
    /*debug*/ m_log.set_fun("cmp_prodprec");

    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;

    /*debug*/ m_log.trace(0) << m_log.op("iter") << m_log.chl << ".m_productions\n";
    for (prod_iter i = m_productions.begin(); i < m_productions.end(); i++) {
        std::shared_ptr<gproduction> production = *i;
        assert(production.get());
        /*debug*/ m_log.trace(0) << m_log.op("produc") << m_log.chl;
        /*debug*/ m_log.out << production->microdump() << "\n";

        if (production->precedence() == 0) {
            auto symbol = production->find_rightmost_terminal_symbol();
            if (symbol.get()) {
                /*debug*/ m_log.trace(1) << m_log.op("rmost") << m_log.cwhite;
                /*debug*/ m_log.out << symbol->identifier() << " " << symbol->precedence() << "\n";

                production->set_precedence_symbol(symbol);
                /*debug*/ m_log.trace(1) << m_log.op("test") << m_log.cwhite;
                /*debug*/ m_log.out << production->precedence() << "\n";
            }
        }
    }
}

void ggenerator::generate_states() {
    /*debug*/ m_log.set_fun("gen_states");

    assert(m_start_symbol.get());
    assert(m_end_symbol.get());
    assert(m_states.empty());

    if (!m_start_symbol->productions().empty()) {

        std::shared_ptr<gstate> start_state = std::make_shared<gstate>();
        start_state->add_item(m_start_symbol->productions().front(), 0);

        /*debug*/ m_log.set_fun("gen_states");
        /*debug*/ m_log.trace(0) << m_log.op("start");
        /*debug*/ m_log.out << m_log.chl << "<" << start_state.use_count() << "> ";
        /*debug*/ m_log.out << start_state->microdump() << "\n";

        clojure(start_state);

        /*debug*/ m_log.set_fun("gen_states");
        /*debug*/ m_log.trace(0) << m_log.op("test");
        /*debug*/ m_log.out << m_log.cwhite << "<" << start_state.use_count() << "> ";
        /*debug*/ m_log.out << start_state->microdump() << "\n";

        m_start_state = start_state;    // moved 10 lines above
        m_states.insert(m_start_state); // m_states.insert(start_state);

        /*debug*/ m_log.trace(1) << m_log.op("push") << ".m_states <- (start_state)\n";
        /*debug*/ m_log.trace(1) << m_log.op("test");
        /*debug*/ m_log.out << m_log.cwhite << "<" << m_states.begin()->use_count() << "> ";
        /*debug*/ m_log.out << m_states.begin()->get()->microdump() << "\n";

        // m_start_state = start_state;

        std::set<std::shared_ptr<gsymbol>, gsymbolc> lookahead_symbols;
        lookahead_symbols.insert(m_end_symbol);

        /*debug*/ m_log.trace(1) << m_log.op("set") << "(lookahead_symbols) <- .m_end_symbol\n";
        /*debug*/ m_log.trace(1) << m_log.op("test");
        /*debug*/ m_log.out << m_log.chl << "<" << m_end_symbol.use_count() << "> ";
        /*debug*/ m_log.out << m_end_symbol->microdump() << "\n";
        /*debug*/ m_log.trace(1) << m_log.op("");
        /*debug*/ m_log.out << m_log.cwhite << "<" << lookahead_symbols.begin()->use_count() << "> ";
        /*debug*/ m_log.out << lookahead_symbols.begin()->get()->microdump() << "\n";

        // start_state->add_lookahead_symbols(...)
        m_start_state->add_lookahead_symbols(m_start_symbol->productions().front(), 0, lookahead_symbols);

        // clang-format off
        /*debug*/ m_log.trace(1) << m_log.op("iter") << "(start_state).items.lookaheads\n";
        /*debug*/ for (auto i : start_state->items()) {
        /*debug*/     m_log.trace(1) << m_log.op("item") << m_log.chl;
        /*debug*/     m_log.out << i.microdump()<<"\n";
        /*debug*/     for (auto l : i.lookahead_symbols()) {
        /*debug*/         m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/         m_log.out << "<" << l.use_count() << "> ";
        /*debug*/         m_log.out << l->microdump() << "\n";
        /*debug*/     }
        /*debug*/ }
        // clang-format on

        // TODO do-while
        int added = 1;
        while (added > 0) {
            added = 0;

            /*debug*/ m_log.trace(1) << m_log.op("iter") << ".m_states\n";
            using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::const_iterator;
            for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {

                const std::shared_ptr<gstate> &state = *i;
                assert(state.get());
                /*debug*/ m_log.trace(1) << m_log.op("state") << m_log.chl;
                /*debug*/ m_log.out << "<" << state.use_count() << "> ";
                /*debug*/ m_log.out << state->microdump() << "\n";

                if (!state->processed()) {

                    state->set_processed(true);
                    /*debug*/ m_log.trace(1) << m_log.op("set") << "(state).processed " << m_log.cwhite;
                    /*debug*/ m_log.out << (state->processed() ? "true" : "false") << "\n";

                    /*debug*/ m_log.trace(1) << m_log.op("iter") << ".m_symbols\n";
                    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
                    for (symb_iter j = m_symbols.begin(); j != m_symbols.end(); ++j) {

                        std::shared_ptr<gsymbol> symbol = *j;
                        assert(symbol.get());
                        /*debug*/ m_log.set_fun("gen_states");
                        /*debug*/ m_log.trace(0) << m_log.op("symbol") << m_log.chl;
                        /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
                        /*debug*/ m_log.out << symbol->microdump() << "\n";

                        if (symbol != m_end_symbol) {
                            /*debug*/ m_log.trace(1) << m_log.op("if") << m_log.ccyan;
                            /*debug*/ m_log.out << symbol->identifier() << " != ";
                            /*debug*/ m_log.out << m_end_symbol->identifier() << "\n";

                            // TODO FIXME check if *symbol == *symbol.get()
                            std::shared_ptr<gstate> goto_state = goto_(state, *symbol);
                            /*debug*/ m_log.set_fun("gen_states");
                            /*debug*/ m_log.trace(0) << m_log.op("goto") << m_log.chl;
                            /*debug*/ m_log.out << "<" << goto_state.use_count() << "> ";
                            /*debug*/ m_log.out << goto_state->microdump() << "\n";

                            if (!goto_state->items().empty()) {

                                std::shared_ptr<gstate> actual_goto_state = *m_states.insert(goto_state).first;
                                /*debug*/ m_log.trace(1) << m_log.op("actual") << m_log.chl;
                                /*debug*/ m_log.out << "<" << actual_goto_state.use_count() << "> ";
                                /*debug*/ m_log.out << actual_goto_state->microdump() << "\n";

                                /*debug*/ m_log.trace(1) << m_log.op("if") << m_log.ccyan;
                                /*debug*/ m_log.out << goto_state << " == " << actual_goto_state;
                                /*debug*/ m_log.out << " " << (goto_state == actual_goto_state ? 1 : 0) << "\n";

                                added += goto_state == actual_goto_state ? 1 : 0;
                                state->add_transition(symbol, actual_goto_state);
                            }
                        }
                        /*debug*/ m_log.trace(1) << m_log.op("test") << "added: " << m_log.cwhite << added << "\n";
                    }
                }
            }
        }
        // clang-format off
        /*debug*/ m_log.trace(1) << m_log.op("get") << ".m_states.indices\n";
        /*debug*/ for (auto s : m_states) {
        /*debug*/     m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/     m_log.out << "<" << s.use_count() << "> " << s->microdump() << "\n";
        /*debug*/ }
        // clang-format on
        generate_indices_for_states();
        // clang-format off
        /*debug*/ m_log.trace(1) << m_log.op("get") << ".m_states.indices\n";
        /*debug*/ for (auto s : m_states) {
        /*debug*/     m_log.trace(1) << m_log.op("") << m_log.chl;
        /*debug*/     m_log.out << "<" << s.use_count() << "> " << s->microdump() << "\n";
        /*debug*/ }
        // clang-format on

        // TODO do-while
        added = 1;
        while (added > 0) {
            added = 0;

            using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::const_iterator;
            for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {
                std::shared_ptr<gstate> state = *i; // maybe weak_ptr
                assert(state.get());

                added += lookahead_clojure(state);
                added += lookahead_goto(state);
            }
        }
        // clang-format off
        /*debug*/ m_log.trace(1) << m_log.op("get") << ".m_states.indices\n";
        /*debug*/ for (auto s : m_states)
        /*debug*/     s->json(0, true, 0, true, s.use_count());
        // clang-format on

        // generate_reduce_transitions();
        // generate_indices_for_transitions();
    }
}

void ggenerator::clojure(const std::shared_ptr<gstate> &state) {
    /*debug*/ m_log.set_fun("clojure");

    assert(state.get());
    /*debug*/ m_log.trace(0) << m_log.op("<state") << m_log.chl;
    /*debug*/ m_log.out << "<" << state.use_count() << "> ";
    /*debug*/ m_log.out << state->microdump() << "\n";

    // TODO do-while
    /*debug*/ m_log.trace(1) << m_log.op("iter") << "until no changes in (state).items\n";
    int added = 1;
    while (added > 0) {
        added = 0;

        std::set<gitem> items = state->items();

        /*debug*/ m_log.trace(1) << m_log.op("iter") << "(state).items[" << items.size() << "]\n";
        using item_iter = std::set<gitem>::const_iterator;
        for (item_iter item = items.begin(); item != items.end(); ++item) {
            /*debug*/ m_log.trace(1) << m_log.op("item") << m_log.chl;
            /*debug*/ m_log.out << item->microdump() << "\n";

            std::shared_ptr<gsymbol> symbol = item->production()->symbol_by_position(item->position());
            /*debug*/ if (!symbol.get())
                /*debug*/ m_log.trace(1) << m_log.op("symbol") << m_log.chl << "nullptr\n";

            // != nullptr
            if (symbol.get()) {
                /*debug*/ m_log.trace(1) << m_log.op("symbol") << m_log.chl;
                /*debug*/ m_log.out << "<" << symbol.use_count() << "> ";
                /*debug*/ m_log.out << symbol->microdump() << "\n";

                const std::vector<std::shared_ptr<gproduction>> &productions = symbol->productions();

                /*debug*/ m_log.trace(1) << m_log.op("iter") << "(symbol).productions[" << productions.size() << "]\n";
                using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
                for (prod_iter j = productions.begin(); j != productions.end(); ++j) {

                    std::shared_ptr<gproduction> production = *j; // TODO maybe weak_ptr
                    assert(production.get());
                    /*debug*/ m_log.trace(1) << m_log.op("produc") << m_log.chl;
                    /*debug*/ m_log.out << "<" << production.use_count() << "> ";
                    /*debug*/ m_log.out << production->microdump() << "\n";

                    added += state->add_item(production, 0);
                    /*debug*/ m_log.trace(1) << m_log.op("add") << m_log.cwhite << added << "\n";
                }
            }
        }
        /*debug*/ m_log.trace(1) << m_log.op("test") << "added: " << m_log.chl << added << "\n";
    }
}

std::shared_ptr<gstate> ggenerator::goto_(const std::shared_ptr<gstate> &state, const gsymbol &symbol) {
    /*debug*/ m_log.set_fun("goto_");
    assert(state.get());

    /*debug*/ m_log.trace(0) << m_log.op("<state");
    /*debug*/ m_log.out << m_log.chl << "<" << state.use_count() << "> ";
    /*debug*/ m_log.out << state->microdump() << "\n";

    std::shared_ptr<gstate> goto_state(new gstate());
    /*debug*/ m_log.trace(1) << m_log.op("new");
    /*debug*/ m_log.out << m_log.chl << "<" << goto_state.use_count() << "> ";
    /*debug*/ m_log.out << goto_state->microdump() << "\n";

    const std::set<gitem> &items = state->items();

    /*debug*/ m_log.trace(1) << m_log.op("iter") << "(<state).items\n";
    using item_iter = std::set<gitem>::const_iterator;
    for (item_iter item = items.begin(); item != items.end(); ++item) {
        /*debug*/ m_log.trace(1) << m_log.op("item") << m_log.chl;
        /*debug*/ m_log.out << item->microdump() << "\n";

        if (item->next_node(symbol)) {

            goto_state->add_item(item->production(), item->position() + 1);
            /*debug*/ m_log.trace(1) << m_log.op("add") << "(goto_state).items <- item\n";
        }
    }

    clojure(goto_state);

    return goto_state;
}

void ggenerator::generate_indices_for_states() {
    int index = 0;

    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::iterator;
    for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {
        std::shared_ptr<gstate> state = *i;
        assert(state.get());

        state->set_index(index);
        ++index;
    }
}

int ggenerator::lookahead_clojure(const std::shared_ptr<gstate> &state) const {
    assert(state.get());

    int added = 0;

    const std::set<gitem> &items = state->items();

    using item_iter = std::set<gitem>::const_iterator;
    for (item_iter item = items.begin(); item != items.end(); ++item) {

        // maybe weak_ptr or const &
        std::shared_ptr<gsymbol> symbol = item->production()->symbol_by_position(item->position());

        // != nullptr
        if (symbol.get()) {
            std::set<std::shared_ptr<gsymbol>, gsymbolc> lookahead_symbols = lookahead(*item);

            const std::vector<std::shared_ptr<gproduction>> &productions = symbol->productions();

            using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
            for (prod_iter j = productions.begin(); j != productions.end(); ++j) {

                std::shared_ptr<gproduction> production = *j; // maybe weak_ptr or const &
                assert(production.get());

                added += state->add_lookahead_symbols(production, 0, lookahead_symbols);
            }
        }
    }

    return added;
}

int ggenerator::lookahead_goto(const std::shared_ptr<gstate> &state) const {
    assert(state.get());

    int added = 0;

    const std::set<gtransition> &transitions = state->transitions();

    using trans_iter = std::set<gtransition>::const_iterator;
    for (trans_iter transition = transitions.begin(); transition != transitions.end(); ++transition) {

        std::shared_ptr<gsymbol> symbol = transition->symbol(); // maybe weak_ptr or const &
        assert(symbol.get());

        const std::set<gitem> &items = state->items();

        using item_iter = std::set<gitem>::const_iterator;
        for (item_iter item = items.begin(); item != items.end(); ++item) {

            int position = item->position();

            // TODO FIXME check comparison between smart pointers
            if (item->production()->symbol_by_position(position) == symbol) {

                std::shared_ptr<gstate> goto_state = transition->state(); // maybe weak_ptr or const &
                added += goto_state->add_lookahead_symbols(item->production(), position + 1, item->lookahead_symbols());
            }
        }
    }

    return added;
}

std::set<std::shared_ptr<gsymbol>, gsymbolc> ggenerator::lookahead(const gitem &item) const {
    std::set<std::shared_ptr<gsymbol>, gsymbolc> lookahead_symbols;

    std::shared_ptr<gproduction> production = item.production(); // maybe weak_ptr or const &
    assert(production.get());

    const std::vector<std::shared_ptr<gsymbol>> &symbols = production->symbols();

    std::vector<std::shared_ptr<gsymbol>>::const_iterator i = symbols.begin() + item.position();

    if (i != symbols.end())
        ++i;

    while (i != symbols.end() && i->get()->nullable()) {
        std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr or const &
        assert(symbol.get());

        lookahead_symbols.insert(symbol->first().begin(), symbol->first().end());

        ++i;
    }

    if (i != symbols.end()) {
        std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr or const &
        assert(symbol.get());

        lookahead_symbols.insert(symbol->first().begin(), symbol->first().end());
    } else {
        lookahead_symbols.insert(item.lookahead_symbols().begin(), item.lookahead_symbols().end());
    }

    return lookahead_symbols;
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
