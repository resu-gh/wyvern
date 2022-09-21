#include "include/gcompiler.hpp"
#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gparser.hpp"
#include "include/paction.hpp"
#include "include/pstate.hpp"
#include "include/pstatemachine.hpp"
#include "include/ptransition.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

gcompiler::gcompiler()
    : m_grammar(std::make_shared<ggrammar>()),
      m_generator(std::make_shared<ggenerator>()),
      m_parser_state_machine(std::make_shared<pstatemachine>()),
      m_actions(std::make_shared<std::vector<paction>>()),
      m_symbols(std::make_shared<std::vector<psymbol>>()),
      m_transitions(std::make_shared<std::vector<ptransition>>()),
      m_states(std::make_shared<std::vector<pstate>>()),
      m_log("yyv", "gcomp", 45) {}

int gcompiler::compile(std::string::iterator &begin, std::string::iterator &end) {

    /*debug*/ std::string h = m_log.hook("compile");
    /*debug*/ m_log.htrace(h, "interval (params)") << "[begin, end) ";
    /*debug*/ m_log.out << m_log.chl << "[" << (void *)&*begin;
    /*debug*/ m_log.out << ", " << (void *)&*end << ")\n";
    /*debug*/ m_log.out << m_log.op("grammar (shared owned)") << "address ";
    /*debug*/ m_log.out << m_log.chl << &*m_grammar << m_log.cnr << " use_count ";
    /*debug*/ m_log.out << m_log.chl << m_grammar.use_count() << "\n";

    gparser parser;
    int errors = parser.parse(begin, end, m_grammar);

    if (errors == 0) {
        // ggenerator generator;
        errors = m_generator->generate(m_grammar);

        if (errors == 0) {
            /*debug*/ m_log.htrace(h, "populating parser state machine") << "\n";
            populate_parser_state_machine();
        }
    }

    // clang-format off
    /*debug*/ m_log.out << m_log.op("gcompiler.actions") << "\n";
    /*debug*/ for (auto a : *m_actions)
    /*debug*/     a.json(0, false, 0, true);
    /*debug*/ m_log.out << m_log.op("gcompiler.symbols") << "\n";
    /*debug*/ for (auto s : *m_symbols)
    /*debug*/     s.json(0, false, 0, true);
    /*debug*/ m_log.out << m_log.op("gcompiler.transitions") << "\n";
    /*debug*/ for (auto t : *m_transitions)
    /*debug*/     t.json(0, false, 0, true);
    /*debug*/ m_log.out << m_log.op("gcompiler.states") << "\n";
    /*debug*/ for (auto s : *m_states)
    /*debug*/     s.json(0, false, 0, true);
    // clang-format on

    return errors;
}

const std::shared_ptr<pstatemachine> &gcompiler::parser_state_machine() const {
    return m_parser_state_machine;
}

const std::string &gcompiler::add_string(const std::string &string) {
    m_strings.push_back(string);
    return m_strings.back();
}

void gcompiler::populate_parser_state_machine() {
    /*debug*/ std::string h = m_log.hook("build_parser_au");

    const std::vector<std::shared_ptr<gaction>> &grammar_actions = m_generator->actions();

    int actions_size = int(grammar_actions.size());
    std::shared_ptr<std::vector<paction>> actions = std::make_shared<std::vector<paction>>(actions_size);
    assert(actions_size == int(actions->size()));

    for (int i = 0; i < actions_size; ++i) {
        const std::shared_ptr<gaction> &grammar_action = grammar_actions[i];
        assert(grammar_action.get());

        paction &action = actions->at(i);
        action.index = grammar_action->index();
        action.identifier = add_string(grammar_action->identifier());
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "ggenerator.actions") << "\n";
    /*debug*/ for (auto a : grammar_actions)
    /*debug*/     a->json(0, false, 0, true, a.use_count());
    /*debug*/ m_log.out << m_log.op("computed actions") << "\n";
    /*debug*/ for (paction a : *actions)
    /*debug*/     a.json(0, false, 0, true);
    // clang-format on

    const std::vector<std::shared_ptr<gsymbol>> &grammar_symbols = m_generator->symbols();

    int symbols_size = int(grammar_symbols.size());
    std::shared_ptr<std::vector<psymbol>> symbols = std::make_shared<std::vector<psymbol>>(symbols_size);
    assert(symbols_size == int(symbols->size()));

    for (int i = 0; i < symbols_size; ++i) {
        const std::shared_ptr<gsymbol> &source_symbol = grammar_symbols[i];
        assert(source_symbol);

        psymbol &symbol = symbols->at(i);
        symbol.index = source_symbol->index();
        symbol.identifier = add_string(source_symbol->identifier());
        symbol.lexeme = add_string(source_symbol->lexeme());
        symbol.type = source_symbol->symbol_type();
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "ggenerator.symbols") << "\n";
    /*debug*/ for (auto s : grammar_symbols)
    /*debug*/     s->json(0, false, 0, true, s.use_count());
    /*debug*/ m_log.out << m_log.op("computed symbols") << "\n";
    /*debug*/ for (psymbol s : *symbols)
    /*debug*/     s.json(0, false, 0, true);
    // clang-format on

    const std::set<std::shared_ptr<gstate>, gstatec> &grammar_states = m_generator->states();

    int states_size = int(grammar_states.size());
    std::shared_ptr<std::vector<pstate>> states = std::make_shared<std::vector<pstate>>(states_size);
    assert(states_size == states->size());

    int transitions_size = 0;
    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::iterator;
    for (state_iter i = grammar_states.begin(); i != grammar_states.end(); ++i) {
        const std::shared_ptr<gstate> &grammar_state = *i;
        assert(grammar_state.get());

        transitions_size += int(grammar_state->transitions().size());
    }

    std::shared_ptr<std::vector<ptransition>> transitions = std::make_shared<std::vector<ptransition>>(transitions_size);
    assert(transitions_size == transitions->size());

    // TODO start state
    int state_index = 0;
    int transition_index = 0;
    for (state_iter i = grammar_states.begin(); i != grammar_states.end(); ++i) {
        const std::shared_ptr<gstate> &grammar_state = *i;
        assert(grammar_state.get());

        pstate &state = (*states)[state_index];
        assert(&state); // TODO maybe useless ref can't be null

        const std::set<gtransition> &source_transitions = grammar_state->transitions();

        state.index = state_index;
        state.transitions_begin = transitions->begin() + transition_index;

        for (auto j = source_transitions.begin(); j != source_transitions.end(); ++j) {
            const gtransition &source_transition = *j;
            assert(&source_transition); // TODO maybe useless

            const std::shared_ptr<gsymbol> &source_symbol = source_transition.symbol();
            assert(source_symbol.get());

            const std::shared_ptr<gstate> &state_transitioned_to = source_transition.state();
            const std::shared_ptr<gsymbol> &reduced_symbol = source_transition.reduced_symbol();

            ptransition &transition = (*transitions)[transition_index];

            // TODO check
            transition.symbol = std::make_shared<psymbol>((*symbols)[source_symbol->index()]);
            transition.state = state_transitioned_to ? std::make_shared<pstate>((*states)[state_transitioned_to->index()]) : nullptr;
            transition.reduced_symbol = reduced_symbol ? std::make_shared<psymbol>((*symbols)[reduced_symbol->index()]) : nullptr;
            // TODO check

            transition.reduced_length = source_transition.reduced_length();
            transition.precedence = source_transition.precedence();
            transition.action = source_transition.action();
            transition.type = source_transition.type();
            transition.index = transition_index;

            ++transition_index;
        }

        state.transitions_end = transitions->begin() + transition_index;

        ++state_index;
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "ggenerator.states") << "\n";
    /*debug*/ for (auto s : grammar_states)
    /*debug*/     s->json(0, false, 0, true, s.use_count());
    /*debug*/ m_log.out << m_log.op("computed states") << "\n";
    /*debug*/ for (pstate s : *states)
    /*debug*/     s.json(0, false, 0, true);
    // clang-format on

    m_parser_state_machine->identifier = add_string(m_grammar->identifier());
    set_actions(actions, actions_size);
    set_symbols(symbols, symbols_size);
    set_transitions(transitions, transitions_size);
    set_states(states, states_size);

    /*debug*/ assert(actions == nullptr);
    /*debug*/ m_log.htrace(h, "actions (after swap)") << m_log.chl << "== nullptr\n";
    /*debug*/ assert(symbols == nullptr);
    /*debug*/ m_log.out << m_log.op("symbols (after swap)") << m_log.chl << "== nullptr\n";
    /*debug*/ assert(transitions == nullptr);
    /*debug*/ m_log.out << m_log.op("transitions (after swap)") << m_log.chl << "== nullptr\n";
}

void gcompiler::set_actions(std::shared_ptr<std::vector<paction>> &actions, int actions_size) {
    assert(m_parser_state_machine.get());
    assert(actions.get() || actions_size == 0);
    assert(actions_size >= 0);
    m_actions = std::move(actions);
    m_parser_state_machine->actions_begin = m_actions->begin();
    m_parser_state_machine->actions_end = m_actions->end();
}

void gcompiler::set_symbols(std::shared_ptr<std::vector<psymbol>> &symbols, int symbols_size) {
    assert(m_parser_state_machine.get());
    assert(symbols.get());
    assert(symbols_size >= 3);
    m_symbols = std::move(symbols);
    m_parser_state_machine->symbols_begin = m_symbols->begin();
    m_parser_state_machine->symbols_end = m_symbols->end();
    // TODO start, end, error symbols
}

void gcompiler::set_transitions(std::shared_ptr<std::vector<ptransition>> &transitions, int transitions_size) {
    assert(transitions.get());
    assert(transitions_size >= 0);
    m_transitions = std::move(transitions);
    m_parser_state_machine->transitions_begin = m_transitions->begin();
    m_parser_state_machine->transitions_end = m_transitions->end();
}

void gcompiler::set_states(std::shared_ptr<std::vector<pstate>> &states, int states_size) {
    // TODO insert param start_state
    assert(states.get());
    assert(states_size >= 0);
    // assert(start_state);
    m_states = std::move(states);
    m_parser_state_machine->states_begin = m_states->begin();
    m_parser_state_machine->states_end = m_states->end();
}
