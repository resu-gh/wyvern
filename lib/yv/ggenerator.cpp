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
#include <string>
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
    /*debug*/ std::string h = m_log.hook("generate");

    // grammar->dump(true);

    m_grammar = grammar;
    m_identifier = m_grammar->identifier();       // if useless remove grammar shared ref
    m_actions.swap(m_grammar->actions());         // ownership moved
    m_productions.swap(m_grammar->productions()); // ownership moved
    m_symbols.swap(m_grammar->symbols());         // ownership moved
    m_start_symbol = m_grammar->start_symbol();   // TODO if useless remove grammar shared ref (move)
    m_end_symbol = m_grammar->end_symbol();       // TODO if useless remove grammar shared ref (move)
    m_error_symbol = m_grammar->error_symbol();   // TODO if useless remove grammar shared ref (move)
    m_errors = 0;                                 // init error
    m_states.clear();                             // TODO maybe can be avoided

    /*debug*/ m_log.htrace(h, "copy") << "grammar -> " << m_log.chl << "m_grammar (shared owned)\n";
    /*debug*/ m_log.out << m_log.op("copy") << "grammar.identifier -> " << m_log.chl << "m_identifier (string owned)\n";
    /*debug*/ m_log.out << m_log.op("move") << "grammar.actions -> " << m_log.chl << "m_actions (vector shared owned)\n";
    /*debug*/ m_log.out << m_log.op("move") << "grammar.productions -> " << m_log.chl << "m_productions (vector shared owned)\n";
    /*debug*/ m_log.out << m_log.op("move") << "grammar.symbols -> " << m_log.chl << "m_symbols (vector shared owned)\n";
    /*debug*/ m_log.out << m_log.op("copy (move?)") << "grammar.start_symbol -> " << m_log.chl << "m_start_symbol (shared owned)\n";
    /*debug*/ m_log.out << m_log.op("copy (move?)") << "grammar.end_symbol -> " << m_log.chl << "m_end_symbol (shared owned)\n";
    /*debug*/ m_log.out << m_log.op("copy (move?)") << "grammar.error_symbol -> " << m_log.chl << "m_error_symbol (shared owned)\n";
    /*debug*/ m_log.out << m_log.op("initialize") << "m_errors = " << m_log.chl << "0\n";
    /*debug*/ m_log.out << m_log.op("reset size of") << "m_states = " << m_log.chl << m_states.size() << "\n";

    // dump(true);
    // m_grammar->dump(true);

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
        generate_states(); // generate_states( m_start_symbol, m_end_symbol, m_symbols );
    }

    // TODO why this strange swap
    int errors = m_errors;
    m_errors = 0;

    return errors;
}

const std::vector<std::shared_ptr<gaction>> &ggenerator::actions() const {
    return m_actions;
}

const std::vector<std::shared_ptr<gsymbol>> &ggenerator::symbols() const {
    return m_symbols;
}

const std::set<std::shared_ptr<gstate>, gstatec> &ggenerator::states() const {
    return m_states;
}

const std::shared_ptr<gstate> &ggenerator::start_state() const {
    return m_start_state;
}

/// calculate identifiers for all symbols
void ggenerator::calculate_identifiers() {
    /*debug*/ std::string h = m_log.hook("cmp_idents");

    /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

        assert(i->get());
        /*debug*/ m_log.out << m_log.op("processing symbol");
        /*debug*/ i->get()->json(0, false, 0, true, i->use_count());

        i->get()->calculate_identifier();
        /*debug*/ m_log.out << m_log.op("computed identifier");
        /*debug*/ i->get()->json(0, false, 0, true, i->use_count());
    }
}

/// check for symbols in the grammar that
/// are referenced but never defined
void ggenerator::check_for_undefined_symbol_errors() {
    /*debug*/ std::string h = m_log.hook("ck_undef_sym");

    if (m_errors == 0) {

        /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
        using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

            assert(i->get());

            std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr
            /*debug*/ m_log.out << m_log.op("processing symbol");
            /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

            if (symbol->symbol_type() == gsymboltype::SYMBOL_NON_TERMINAL && symbol->productions().empty() && symbol->precedence() <= 0) {
                ++m_errors;
                /*error*/ m_log.ehtrace(h, "ERROR ck_undef_sym()") << "type: " << ecode::E_PARSER_UNDEF_SYMBOL << ", ";
                /*error*/ m_log.err << "line: " << symbol->line() << ", #errors: " << m_errors << ", ";
                /*error*/ m_log.err << "undefined symbol `" << m_log.cwhite << symbol->identifier() << m_log.cred << "`\n";
            }
        }
    }
}

/// check for symbols in the grammar that are defined but never referenced
void ggenerator::check_for_unreferenced_symbol_errors() {
    /*debug*/ std::string h = m_log.hook("ck_unref_sym");

    if (m_errors == 0) {

        /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
        using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

            assert(i->get());

            int references = 0;

            std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr
            /*debug*/ m_log.out << m_log.op("processing symbol");
            /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

            if (symbol != m_start_symbol && symbol != m_end_symbol && symbol->lexeme() != m_error_symbol->lexeme()) {

                /*debug*/ m_log.out << m_log.op("iterate m_productions") << "\n";
                using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
                for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {

                    assert(i->get());

                    std::shared_ptr<gproduction> production = *i; // maybe weak_ptr
                    /*debug*/ m_log.out << m_log.op("processing production");
                    /*debug*/ production->json(0, false, 0, true, production.use_count());

                    if (production->symbol()->symbol_type() != gsymboltype::SYMBOL_TERMINAL)
                        references += production->count_references_to_symbol(symbol);
                }

                if (references == 0) {
                    ++m_errors;
                    /*error*/ m_log.ehtrace(h, "ERROR ck_unref_sym()") << "type: " << ecode::E_PARSER_UNREF_SYMBOL << ", ";
                    /*error*/ m_log.err << "line: " << symbol->line() << ", #errors: " << m_errors << ", ";
                    /*error*/ m_log.err << "unreferenced symbol `" << m_log.cwhite << symbol->identifier() << m_log.cred << "`\n";
                }
            }
        }
    }
}

/// check for the error symbol being
/// used in the lhs of a production
void ggenerator::check_for_error_symbol_on_left_hand_side_errors() {
    /*debug*/ std::string h = m_log.hook("ck_error_lhs");

    assert(m_error_symbol.get());

    /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

        assert(i->get());

        std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr
        /*debug*/ m_log.out << m_log.op("processing symbol");
        /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

        if (!symbol->productions().empty() && symbol->lexeme() == m_error_symbol->lexeme()) {
            ++m_errors;
            /*error*/ m_log.ehtrace(h, "ERROR ck_error_lhs()") << "type: " << ecode::E_PARSER_ERROR_ON_LHS << ", ";
            /*error*/ m_log.err << "line: " << symbol->line() << ", #errors: " << m_errors << ", ";
            /*error*/ m_log.err << "the `" << m_log.cwhite << symbol->lexeme() << m_log.cred << "` symbol appears on the lhs of this production\n";
        }
    }
}

/// calculate which symbols are terminal and non-terminal
/// any symbol w/ 1 or more productions are assumed to be non-terminal
/// any symbol w/ no productions are assumed to be terminal
/// (.start, .end and .error are not processed)
void ggenerator::calculate_terminal_and_non_terminal_symbols() {
    /*debug*/ std::string h = m_log.hook("cmp_t_nt");

    /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

        assert(i->get());

        std::shared_ptr<gsymbol> symbol = *i; // maybe weak_ptr
        /*debug*/ m_log.out << m_log.op("processing symbol");
        /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

        /// i need to skip over .start, .end and .error (description)
        /// TODO added
        // if (symbol == m_start_symbol || symbol == m_end_symbol || symbol == m_error_symbol)
        //     continue;
        /// TODO added

        /// FIXME very strange check
        if (symbol->symbol_type() == gsymboltype::SYMBOL_NULL) {
            if (symbol->productions().empty()) {
                symbol->set_symbol_type(gsymboltype::SYMBOL_TERMINAL);
                /*debug*/ m_log.out << m_log.op("changed symbol_type") << m_log.cyellow << i->get()->symbol_type() << "\n";
            } else {
                symbol->set_symbol_type(gsymboltype::SYMBOL_NON_TERMINAL);
                /*debug*/ m_log.out << m_log.op("changed symbol_type") << m_log.cyellow << i->get()->symbol_type() << "\n";
            }
        }
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
    /*debug*/ std::string h = m_log.hook("cmp_impl_t");

    /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator;
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

        std::shared_ptr<gsymbol> non_terminal_symbol = *i; // maybe weak_ptr

        // TODO added
        // if (non_terminal_symbol->symbol_type() != gsymboltype::SYMBOL_NON_TERMINAL) {
        //     m_log.out << m_log.cred << "skip " << m_log.chl;
        //     non_terminal_symbol->json(0, false, 0, true, non_terminal_symbol.use_count());
        //     continue;
        // }
        // TODO added

        if (non_terminal_symbol.get() && non_terminal_symbol != m_error_symbol) {
            /*debug*/ m_log.out << m_log.op("processing non-terminal?");
            /*debug*/ non_terminal_symbol->json(0, false, 0, true, non_terminal_symbol.use_count());

            std::shared_ptr<gsymbol> terminal_symbol = non_terminal_symbol->implicit_terminal();

            if (terminal_symbol) {
                assert(terminal_symbol.get() != non_terminal_symbol.get());

                /*debug*/ m_log.out << m_log.op("implicit terminal found");
                /*debug*/ terminal_symbol->json(0, false, 0, true, terminal_symbol.use_count());

                terminal_symbol->replace_by_non_terminal(non_terminal_symbol);
                /*debug*/ m_log.out << m_log.op("replaced with");
                /*debug*/ terminal_symbol->json(0, false, 0, true, terminal_symbol.use_count());

                replace_references_to_symbol(non_terminal_symbol, terminal_symbol);

                /*debug*/ m_log.htrace(h, "reset symbol") << m_log.cred << "X ";
                /*debug*/ i->get()->json(0, false, 0, true, i->use_count());
                i->reset();
            }
        }
    }

    symb_iter i = m_symbols.begin();
    while (i != m_symbols.end())
        if (!i->get()) { // i->get() == nullptr
            /*debug*/ m_log.out << m_log.op("erasing nullptr ref") << m_log.cred << "X\n";
            i = m_symbols.erase(i);
        } else {
            /*debug*/ m_log.out << m_log.op("skipping symbol");
            /*debug*/ i->get()->json(0, false, 0, true, i->use_count());
            ++i;
        }

    // i = (!i->get()) ? m_symbols.erase(i) : i + 1;
}

void ggenerator::replace_references_to_symbol(const std::shared_ptr<gsymbol> &to_symbol, const std::shared_ptr<gsymbol> &with_symbol) {
    /*debug*/ std::string h = m_log.hook("repl_symb_refs");

    /*debug*/ m_log.htrace(h, "iterate m_productions") << "\n";
    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
    for (prod_iter i = m_productions.begin(); i != m_productions.end(); ++i) {

        std::shared_ptr<gproduction> production = *i; // TODO maybe weak_ptr or const &
        assert(production.get());
        /*debug*/ m_log.out << m_log.op("processing production");
        /*debug*/ production->json(0, false, 0, true, production.use_count());

        production->replace_references_to_symbol(to_symbol, with_symbol);
    }
}

/// calculate the index for each symbol
void ggenerator::calculate_symbol_indices() {
    /*debug*/ std::string h = m_log.hook("cmp_indices");

    int index = 0;

    /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator; // FIXME iter or const_iter ?
    for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

        assert(i->get());

        std::shared_ptr<gsymbol> symbol = *i;
        /*debug*/ m_log.out << m_log.op("processing symbol");
        /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

        symbol->set_index(index);
        /*debug*/ m_log.out << m_log.op("new index for symbol");
        /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

        ++index;
    }
}

/// calculate the first position sets for each symbol
/// until no more terminals can be added to any first
/// position sets
void ggenerator::calculate_first() {
    /*debug*/ std::string h = m_log.hook("cmp_first");

    int added = 1;
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator; // FIXME iter or const_iter ?

    /*debug*/ m_log.htrace(h, "iterate until no changes in first sets") << "\n";

    // TODO do-while
    while (added > 0) {
        added = 0;

        /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

            std::shared_ptr<gsymbol> symbol = *i;
            assert(symbol.get());
            /*debug*/ m_log.out << m_log.op("processing symbol");
            /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

            added += symbol->calculate_first();
            /*debug*/ m_log.out << m_log.op("computed symbol first set") << "\n";
            /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());
        }

        /*debug*/ m_log.out << m_log.op("added symbols") << "added: " << m_log.cwhite << added << "\n";
    }
}

/// calculate the follow positions sets for each symbol
/// until no more terminals can be added to any follow
/// position sets
void ggenerator::calculate_follow() {
    /*debug*/ std::string h = m_log.hook("cmp_follow");

    m_start_symbol->add_symbol_to_follow(m_end_symbol); // TODO maybe useless (repeated inside the loop)

    int added = 1;
    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::iterator;

    /*debug*/ m_log.htrace(h, "iterate until no changes in follow sets") << "\n";

    // TODO do-while
    while (added > 0) {
        added = 0;

        /*debug*/ m_log.htrace(h, "iterate m_symbols") << "\n";
        for (symb_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {

            std::shared_ptr<gsymbol> symbol = *i;
            assert(symbol.get());
            /*debug*/ m_log.out << m_log.op("processing symbol");
            /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

            added += symbol->calculate_follow();
            /*debug*/ m_log.out << m_log.op("computed symbol first set") << "\n";
            /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());
        }

        /*debug*/ m_log.out << m_log.op("added symbols") << "added: " << m_log.cwhite << added << "\n";
    }
}

/// calculate the precedence of each production that
/// hasn't had precedence set explicitly
/// as the precedence of its rightmost terminal
void ggenerator::calculate_precedence_of_productions() {
    /*debug*/ std::string h = m_log.hook("cmp_prodprec");

    using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;

    /*debug*/ m_log.htrace(h, "iterate m_productions") << "\n";
    for (prod_iter i = m_productions.begin(); i < m_productions.end(); i++) {

        std::shared_ptr<gproduction> production = *i; // TODO maybe weak_ptr or const &
        assert(production.get());
        /*debug*/ m_log.out << m_log.op("processing production");
        /*debug*/ production->json(0, false, 0, true, production.use_count());

        if (production->precedence() == 0) {

            std::shared_ptr<gsymbol> symbol = production->find_rightmost_terminal_symbol(); // TODO maybe weak_ptr or const &

            if (symbol.get()) {
                /*debug*/ m_log.out << m_log.op("rightmost terminal symbol");
                /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

                production->set_precedence_symbol(symbol);
                /*debug*/ m_log.out << m_log.op("set rightmost terminal -> production (var) precedence_symbol") << "\n";
                /*debug*/ production->json(0, false, 0, false, production.use_count());
            }
        }
    }
}

void ggenerator::generate_states() {
    /*debug*/ std::string h = m_log.hook("generate_states");

    assert(m_start_symbol.get());
    assert(m_end_symbol.get());
    assert(m_states.empty());

    if (!m_start_symbol->productions().empty()) {

        std::shared_ptr<gstate> start_state = std::make_shared<gstate>();
        /*debug*/ m_log.htrace(h, "new start state (builded)") << "\n";
        /*debug*/ start_state->json(0, false, 0, true, start_state.use_count());

        start_state->add_item(m_start_symbol->productions().front(), 0);
        /*debug*/ m_log.out << m_log.op("add m_start_symbol productions.front() to start_state (var) items in pos 0") << "\n";
        /*debug*/ start_state->json(0, false, 0, false, start_state.use_count());

        clojure(start_state);
        /*debug*/ m_log.htrace(h, "after computing clojure on start_state (var)") << "\n";
        /*debug*/ start_state->json(0, false, 0, false, start_state.use_count());

        m_start_state = start_state; // moved 10 lines above
        /*debug*/ m_log.out << m_log.op("init m_start_state <- start_state (var)") << "\n";
        /*debug*/ m_start_state->json(0, false, 0, true, m_start_state.use_count());

        m_states.insert(m_start_state); // m_states.insert(start_state);
        /*debug*/ m_log.out << m_log.op("add m_start_state to m_states") << "\n";
        /*debug*/ m_states.begin()->get()->json(0, false, 0, true, m_states.begin()->use_count());

        // m_start_state = start_state;

        std::set<std::shared_ptr<gsymbol>, gsymbolc> lookahead_symbols;
        lookahead_symbols.insert(m_end_symbol);
        /*debug*/ m_log.out << m_log.op("add m_end_symbol to lookahead_symbols (var) set") << "\n";
        /*debug*/ lookahead_symbols.begin()->get()->json(0, false, 0, true, lookahead_symbols.begin()->use_count());

        // start_state->add_lookahead_symbols(...)
        m_start_state->add_lookahead_symbols(m_start_symbol->productions().front(), 0, lookahead_symbols);
        /*debug*/ m_log.out << m_log.op("add lookahead_symbols -> m_start_state.item of m_start_symbol.productions.front in position 0") << "\n";
        /*debug*/ m_start_state->items().find(gitem(m_start_symbol->productions().front(), 0))->json(0, false, 0, false);

        /*debug*/ m_log.out << m_log.op("iterate until no more states are pushed in m_states") << "\n";

        // TODO do-while
        int added = 1;
        while (added > 0) {
            added = 0;

            /*debug*/ m_log.out << m_log.op("iterate m_states") << "\n";
            using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::const_iterator;
            for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {

                const std::shared_ptr<gstate> &state = *i;
                assert(state.get());
                /*debug*/ m_log.out << m_log.op("processing state");
                /*debug*/ state->json(0, false, 0, true, state.use_count());

                if (!state->processed()) {

                    state->set_processed(true);
                    /*debug*/ m_log.out << m_log.op("set state (var) processed -> true") << "\n";
                    /*debug*/ state->json(0, false, 0, true, state.use_count());

                    /*debug*/ m_log.out << m_log.op("iterate m_symbols") << "\n";
                    using symb_iter = std::vector<std::shared_ptr<gsymbol>>::const_iterator;
                    for (symb_iter j = m_symbols.begin(); j != m_symbols.end(); ++j) {

                        std::shared_ptr<gsymbol> symbol = *j;
                        assert(symbol.get());
                        /*debug*/ m_log.out << m_log.op("processing symbol") << "\n";
                        /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

                        if (symbol != m_end_symbol) {

                            std::shared_ptr<gstate> goto_state = goto_(state, *symbol);
                            /*debug*/ m_log.htrace(h, "computed goto state") << "\n";
                            /*debug*/ goto_state->json(0, false, 0, false, goto_state.use_count());

                            if (!goto_state->items().empty()) {

                                std::shared_ptr<gstate> actual_goto_state = *m_states.insert(goto_state).first;
                                /*debug*/ m_log.out << m_log.op("add actual_goto_state (var) to m_states if is == to goto_state (var)") << "\n";
                                /*debug*/ actual_goto_state->json(0, false, 0, false, actual_goto_state.use_count());

                                added += goto_state == actual_goto_state ? 1 : 0;

                                state->add_transition(symbol, actual_goto_state);
                                /*debug*/ m_log.out << m_log.op("add a new transition from to state (var) to actual_goto_state (var) on symbol (var)") << "\n";
                                /*debug*/ state->json(0, false, 0, false, state.use_count());
                            }
                        }
                        /*debug*/ m_log.out << m_log.op("added states") << "added: " << m_log.cwhite << added << "\n";
                    }
                }
            }
        }
        generate_indices_for_states();

        /*debug*/ m_log.htrace(h, "iterate until no more states are pushed in m_states") << "\n";

        // TODO do-while
        added = 1;
        while (added > 0) {
            added = 0;

            /*debug*/ m_log.out << m_log.op("iterate m_states") << "\n";
            using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::const_iterator;
            for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {
                std::shared_ptr<gstate> state = *i; // maybe weak_ptr
                assert(state.get());
                /*debug*/ m_log.out << m_log.op("processing state");
                /*debug*/ state->json(0, false, 0, true, state.use_count());

                added += lookahead_clojure(state);
                added += lookahead_goto(state);
            }
        }

        generate_reduce_transitions();
        generate_indices_for_transitions();
    }
}

void ggenerator::clojure(const std::shared_ptr<gstate> &state) {
    /*debug*/ std::string h = m_log.hook("clojure");

    assert(state.get());

    /*debug*/ m_log.htrace(h, "iterate until no changes in state (param) items") << "\n";

    // TODO do-while
    int added = 1;
    while (added > 0) {
        added = 0;

        std::set<gitem> items = state->items();

        /*debug*/ m_log.out << m_log.op("iterate state (param) items") << "\n";
        using item_iter = std::set<gitem>::const_iterator;
        for (item_iter item = items.begin(); item != items.end(); ++item) {

            /*debug*/ m_log.out << m_log.op("processing item");
            /*debug*/ item->json(0, false, 0, true);

            std::shared_ptr<gsymbol> symbol = item->production()->symbol_by_position(item->position());
            /*debug*/ m_log.out << m_log.op("get symbol from item.productions in position item.position") << "\n";

            if (symbol.get()) {
                /*debug*/ m_log.out << m_log.op("processing symbol");
                /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

                const std::vector<std::shared_ptr<gproduction>> &productions = symbol->productions();

                /*debug*/ m_log.out << m_log.op("iterate symbol (var) items") << "\n";
                using prod_iter = std::vector<std::shared_ptr<gproduction>>::const_iterator;
                for (prod_iter j = productions.begin(); j != productions.end(); ++j) {

                    std::shared_ptr<gproduction> production = *j; // TODO maybe weak_ptr or const &
                    assert(production.get());
                    /*debug*/ m_log.out << m_log.op("processing production");
                    /*debug*/ production->json(0, false, 0, true, production.use_count());

                    added += state->add_item(production, 0);
                    /*debug*/ m_log.out << m_log.op("add production (var) to state (param) .items") << "\n";
                }
            }
        }
        /*debug*/ m_log.out << m_log.op("added symbols") << "added: " << m_log.cwhite << added << "\n";
    }
}

std::shared_ptr<gstate> ggenerator::goto_(const std::shared_ptr<gstate> &state, const gsymbol &symbol) {
    /*debug*/ auto h = m_log.hook("goto_");

    assert(state.get());

    std::shared_ptr<gstate> goto_state(new gstate());
    /*debug*/ m_log.htrace(h, "new goto state (builded)") << "\n";
    /*debug*/ goto_state->json(0, false, 0, true, goto_state.use_count());

    const std::set<gitem> &items = state->items();

    /*debug*/ m_log.out << m_log.op("iterate goto_state (var) items") << "\n";
    using item_iter = std::set<gitem>::const_iterator;
    for (item_iter item = items.begin(); item != items.end(); ++item) {
        /*debug*/ m_log.out << m_log.op("processing item");
        /*debug*/ item->json(0, false, 0, true);

        if (item->next_node(symbol)) {

            goto_state->add_item(item->production(), item->position() + 1);
            // FIXME? critical
            /*debug*/ m_log.out << m_log.op("add item to goto_state using item.production and item.position") << "\n";
            /*debug*/ goto_state->items().find(gitem(item->production(), item->position() + 1))->json(0, false, 0, true);
        }
    }

    clojure(goto_state);

    return goto_state;
}

void ggenerator::generate_indices_for_states() {
    /*debug*/ std::string h = m_log.hook("gen_states_idxs");

    int index = 0;

    /*debug*/ m_log.htrace(h, "iterate m_states") << "\n";
    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::iterator;
    for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {
        std::shared_ptr<gstate> state = *i;
        assert(state.get());

        state->set_index(index);
        /*debug*/ m_log.out << m_log.op("set state (var) index");
        /*debug*/ state->json(0, false, 0, true, state.use_count());

        ++index;
    }
}

// TODO comment
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

// TODO comment
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

// TODO comment
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

void ggenerator::generate_reduce_transitions() {
    /*debug*/ std::string h = m_log.hook("gen_reduce_trans");

    /*debug*/ m_log.htrace(h, "iterate m_states") << "\n";
    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::const_iterator;
    for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {

        std::shared_ptr<gstate> state = *i;
        assert(state.get());
        /*debug*/ m_log.out << m_log.op("processing state");
        /*debug*/ state->json(0, false, 0, true, state.use_count());

        /*debug*/ m_log.htrace(h, "iterate state (var) items") << "\n";
        using item_iter = std::set<gitem>::const_iterator;
        for (item_iter item = state->items().begin(); item != state->items().end(); ++item) {
            /*debug*/ m_log.out << m_log.op("processing item");
            /*debug*/ item->json(0, false, 0, true);

            if (item->dot_at_end()) {
                const std::set<std::shared_ptr<gsymbol>, gsymbolc> &symbols = item->lookahead_symbols();

                /*debug*/ m_log.htrace(h, "iterate item (var) lookaheads") << "\n";
                using symb_iter = std::set<std::shared_ptr<gsymbol>, gsymbolc>::const_iterator;
                for (symb_iter j = symbols.begin(); j != symbols.end(); ++j) {

                    std::shared_ptr<gsymbol> symbol = *j;
                    assert(symbol.get());
                    /*debug*/ m_log.out << m_log.op("processing lookahead");
                    /*debug*/ symbol->json(0, false, 0, true, symbol.use_count());

                    generate_reduce_transition(state, symbol, item->production());
                }
            }
        }
    }
}

void ggenerator::generate_reduce_transition(const std::shared_ptr<gstate> &state, const std::shared_ptr<gsymbol> &symbol, const std::shared_ptr<gproduction> &production) {
    /*debug*/ std::string h = m_log.hook("gen_reduce_tran");

    assert(state.get());
    assert(symbol.get());
    assert(production.get());

    std::set<gtransition>::iterator transition = state->find_transition_by_symbol(symbol);

    if (transition == state->transitions().end()) {

        /*debug*/ m_log.htrace(h, "transition on symbol not found") << "\n";
        state->add_transition(symbol, production->symbol(), production->length(), production->precedence(), production->action_index());

        /*debug*/ m_log.out << m_log.op("added transition") << "\n";
        /*debug*/ state->find_transition_by_symbol(symbol)->json(0, false, 0, false);

    } else {

        /*debug*/ m_log.htrace(h, "transition on symbol found") << "\n";
        /*debug*/ transition->json(0, false, 0, false);

        switch (transition->type()) {
        case gtranstype::TRANSITION_SHIFT: {

            /*debug*/ m_log.htrace(h, "multi check (TRANSITION_SHIFT)") << "\n";
            /*debug*/ production->json(0, false, 0, false, production.use_count());
            /*debug*/ m_log.out << m_log.ccyan << "prod.precsymb.prec = " << production->precedence() << "\n";
            /*debug*/ symbol->json(0, false, 0, false, symbol.use_count());
            /*debug*/ m_log.out << m_log.ccyan << "symb.prec = " << symbol->precedence() << "\n";
            /*debug*/ m_log.out << m_log.ccyan << "symb.assoc = " << symbol->associativity() << "\n";

            if (production->precedence() == 0 || symbol->precedence() == 0 || (symbol->precedence() == production->precedence() && symbol->associativity() == gsymbolassoc::ASSOCIATE_NULL)) {

                ++m_errors;
                /*error*/ m_log.ehtrace(h, "ERROR !gen_reduce_tran()") << "type: " << ecode::E_PARSER_PARSE_TABLE_CONFLICT << ", ";
                /*error*/ m_log.err << "#errors: " << m_errors << ", ";
                /*error*/ m_log.err << "shift/reduce conflict for ";
                /*error*/ m_log.err << m_log.cwhite << production->symbol()->identifier();
                /*error*/ m_log.err << m_log.cred << " on ";
                /*error*/ m_log.err << m_log.cwhite << symbol->lexeme() << "\n";

            } else if (production->precedence() > symbol->precedence() || (symbol->precedence() == production->precedence() && symbol->associativity() == gsymbolassoc::ASSOCIATE_RIGHT)) {

                // TODO why not production as parameter
                transition->override_shift_to_reduce(production->symbol(), production->length(), production->precedence(), production->action_index());
            }

            break;
        }

        case gtranstype::TRANSITION_REDUCE: {
            if (production->precedence() == 0 || transition->precedence() == 0 || production->precedence() == transition->precedence()) {

                ++m_errors;
                /*error*/ m_log.ehtrace(h, "ERROR !gen_reduce_tran()") << "type: " << ecode::E_PARSER_PARSE_TABLE_CONFLICT << ", ";
                /*error*/ m_log.err << "#errors: " << m_errors << ", ";
                /*error*/ m_log.err << "reduce/reduce conflict for ";
                /*error*/ m_log.err << m_log.cwhite << production->symbol()->identifier();
                /*error*/ m_log.err << m_log.cred << " and ";
                /*error*/ m_log.err << m_log.cwhite << transition->reduced_symbol()->identifier();
                /*error*/ m_log.err << m_log.cred << " on ";
                /*error*/ m_log.err << m_log.cwhite << symbol->lexeme() << "\n";

            } else if (production->precedence() > transition->precedence()) {

                // TODO why not production as parameter
                transition->override_reduce_to_reduce(production->symbol(), production->length(), production->precedence(), production->action_index());
            }

            break;
        }
        }
    }
}

void ggenerator::generate_indices_for_transitions() {
    /*debug*/ std::string h = m_log.hook("gen_indices_trans");

    /*debug*/ m_log.htrace(h, "iterate m_states") << "\n";
    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::const_iterator;
    for (state_iter i = m_states.begin(); i != m_states.end(); ++i) {

        std::shared_ptr<gstate> state = *i; // TODO maybe weak_ptr or const &
        assert(state.get());

        /*debug*/ m_log.out << m_log.op("generating transitions indices for state") << "\n";
        /*debug*/ state->json(0, false, 0, true, state.use_count());

        state->generate_indices_for_transitions();

        // clang-format off
        /*debug*/ for (auto t : state->transitions())
        /*debug*/     t.json(0, false, 0, true);
        // clang-format off
    }
}

void ggenerator::dump(bool compact) const {
    std::string h = m_log.hook("dump");

    m_log.htrace(h, "m_identifier") << m_log.chl << m_identifier << "\n";

    m_log.out << m_log.op("m_errors") << m_log.chl << m_errors << "\n";

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

    // TODO add states and nullptr checks
}
