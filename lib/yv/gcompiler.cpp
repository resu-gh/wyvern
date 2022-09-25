#include "include/gcompiler.hpp"
#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gparser.hpp"
#include "include/paction.hpp"
#include "include/pstate.hpp"
#include "include/pstatemachine.hpp"
#include "include/ptransition.hpp"
#include "include/xcompiler.hpp"
#include "include/xtoken.hpp"
#include "include/xtokentype.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

gcompiler::gcompiler()
    : m_grammar(std::make_shared<ggrammar>()),
      m_generator(std::make_shared<ggenerator>()),
      m_actions(),
      m_symbols(),
      m_transitions(),
      m_states(),
      m_parser_state_machine(std::make_unique<pstatemachine>()),
      m_lexer(std::make_unique<xcompiler>()),
      m_whitespace_lexer(std::make_unique<xcompiler>()),
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
            /*debug*/ m_log.htrace(h, "populating lexer state machine") << "\n";
            populate_lexer_state_machine();            // TODO skipped errors from xcompiler?
            // /*debug*/ m_log.htrace(h, "populating whitespacelexer state machine") << "\n";
            // populate_whitespace_lexer_state_machine(); // TODO skipped errors from xcompiler?
        }
    }

    return errors;
}

const std::unique_ptr<pstatemachine> &gcompiler::parser_state_machine() const {
    return m_parser_state_machine;
}

const char *gcompiler::add_string(const std::string &string) {
    m_strings.push_back(string);
    return m_strings.back().c_str();
}

void gcompiler::populate_parser_state_machine() {
    /*debug*/ std::string h = m_log.hook("build_parser_au");

    const std::vector<std::shared_ptr<gaction>> &grammar_actions = m_generator->actions();

    int actions_size = int(grammar_actions.size());
    std::unique_ptr<paction[]> actions = std::make_unique<paction[]>(actions_size);

    for (int i = 0; i < actions_size; ++i) {
        const std::shared_ptr<gaction> &grammar_action = grammar_actions[i];
        assert(grammar_action.get());

        paction &action = actions[i];
        action.index = grammar_action->index();
        action.identifier = add_string(grammar_action->identifier());
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "ggenerator.actions") << "\n";
    /*debug*/ for (auto a : grammar_actions)
    /*debug*/     a->json(0, false, 0, true, a.use_count());
    // clang-format on

    const std::vector<std::shared_ptr<gsymbol>> &grammar_symbols = m_generator->symbols();

    int symbols_size = int(grammar_symbols.size());
    std::unique_ptr<psymbol[]> symbols = std::make_unique<psymbol[]>(symbols_size);

    for (int i = 0; i < symbols_size; ++i) {
        const std::shared_ptr<gsymbol> &source_symbol = grammar_symbols[i];
        assert(source_symbol);

        psymbol &symbol = symbols[i];
        symbol.index = source_symbol->index();
        symbol.identifier = add_string(source_symbol->identifier());
        symbol.lexeme = add_string(source_symbol->lexeme());
        symbol.type = source_symbol->symbol_type();
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "ggenerator.symbols") << "\n";
    /*debug*/ for (auto s : grammar_symbols)
    /*debug*/     s->json(0, false, 0, true, s.use_count());
    // clang-format on

    const std::set<std::shared_ptr<gstate>, gstatec> &grammar_states = m_generator->states();

    int states_size = int(grammar_states.size());
    std::unique_ptr<pstate[]> states = std::make_unique<pstate[]>(states_size);

    int transitions_size = 0;
    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::iterator;
    for (state_iter i = grammar_states.begin(); i != grammar_states.end(); ++i) {
        const std::shared_ptr<gstate> &grammar_state = *i;
        assert(grammar_state.get());

        transitions_size += int(grammar_state->transitions().size());
    }

    std::unique_ptr<ptransition[]> transitions = std::make_unique<ptransition[]>(transitions_size);

    const pstate *start_state = nullptr;
    int state_index = 0;
    int transition_index = 0;
    for (state_iter i = grammar_states.begin(); i != grammar_states.end(); ++i) {
        const std::shared_ptr<gstate> &grammar_state = *i;
        assert(grammar_state.get());

        pstate &state = states[state_index];

        const std::set<gtransition> &source_transitions = grammar_state->transitions();

        state.index = state_index;
        state.length = int(source_transitions.size());
        state.transitions = &transitions[transition_index];

        if (grammar_state == m_generator->start_state())
            start_state = &state;

        for (auto j = source_transitions.begin(); j != source_transitions.end(); ++j) {
            const gtransition &source_transition = *j;
            assert(&source_transition); // TODO maybe useless

            const std::shared_ptr<gsymbol> &source_symbol = source_transition.symbol();
            assert(source_symbol.get());

            const std::shared_ptr<gstate> &state_transitioned_to = source_transition.state();
            const std::shared_ptr<gsymbol> &reduced_symbol = source_transition.reduced_symbol();

            ptransition &transition = transitions[transition_index];

            transition.symbol = &symbols[source_symbol->index()];
            transition.state = state_transitioned_to ? &states[state_transitioned_to->index()] : nullptr;
            transition.reduced_symbol = reduced_symbol ? &symbols[reduced_symbol->index()] : nullptr;

            transition.reduced_length = source_transition.reduced_length();
            transition.precedence = source_transition.precedence();
            transition.action = source_transition.action();
            transition.type = source_transition.type();
            transition.index = transition_index;

            ++transition_index;
        }

        ++state_index;
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "ggenerator.states") << "\n";
    /*debug*/ for (auto s : grammar_states)
    /*debug*/     s->json(0, false, 0, true, s.use_count());
    // clang-format on

    m_parser_state_machine->identifier = add_string(m_grammar->identifier());
    set_actions(actions, actions_size);
    set_symbols(symbols, symbols_size);
    set_transitions(transitions, transitions_size);
    set_states(states, states_size, start_state);

    /*debug*/ assert(actions == nullptr);
    /*debug*/ m_log.htrace(h, "actions (after swap)") << m_log.chl << "== nullptr\n";
    /*debug*/ assert(symbols == nullptr);
    /*debug*/ m_log.out << m_log.op("symbols (after swap)") << m_log.chl << "== nullptr\n";
    /*debug*/ assert(transitions == nullptr);
    /*debug*/ m_log.out << m_log.op("transitions (after swap)") << m_log.chl << "== nullptr\n";
}

void gcompiler::set_actions(std::unique_ptr<paction[]> &actions, int actions_size) {
    assert(m_parser_state_machine.get());
    assert(actions.get() || actions_size == 0);
    assert(actions_size >= 0);
    m_actions = std::move(actions);
    m_parser_state_machine->actions_size = actions_size;
    m_parser_state_machine->actions = m_actions.get();
}

void gcompiler::set_symbols(std::unique_ptr<psymbol[]> &symbols, int symbols_size) {
    assert(m_parser_state_machine.get());
    assert(symbols.get());
    assert(symbols_size >= 3);
    m_symbols = std::move(symbols);
    m_parser_state_machine->symbols_size = symbols_size;
    m_parser_state_machine->symbols = m_symbols.get();
    m_parser_state_machine->start_symbol = &m_symbols[0];
    m_parser_state_machine->end_symbol = &m_symbols[1];
    m_parser_state_machine->error_symbol = &m_symbols[2];
}

void gcompiler::set_transitions(std::unique_ptr<ptransition[]> &transitions, int transitions_size) {
    assert(transitions.get());
    assert(transitions_size >= 0);
    m_transitions = std::move(transitions);
    m_parser_state_machine->transitions_size = transitions_size;
    m_parser_state_machine->transitions = m_transitions.get();
}

void gcompiler::set_states(std::unique_ptr<pstate[]> &states, int states_size, const pstate *start_state) {
    assert(states.get());
    assert(states_size >= 0);
    assert(start_state);
    m_states = std::move(states);
    m_parser_state_machine->states = m_states.get();
    m_parser_state_machine->start_state = start_state;
}

/// generate tokens for generating the lexical analyzer from each
/// of the terminal symbols in the grammar
void gcompiler::populate_lexer_state_machine() {
    /*debug*/ std::string h = m_log.hook("build_scanner_au");

    const std::vector<std::shared_ptr<gsymbol>> &grammar_symbols = m_generator->symbols();

    std::vector<xtoken> tokens;
    int column = 1;

    for (std::size_t i = 0; i < grammar_symbols.size(); ++i, ++column) {
        const std::shared_ptr<gsymbol> &grammar_symbol = grammar_symbols[i];
        assert(grammar_symbol.get());

        if (grammar_symbol->symbol_type() == gsymboltype::SYMBOL_TERMINAL) {
            psymbol &symbol = m_symbols[i];

            int line = grammar_symbol->line();
            xtokentype token_type = grammar_symbol->lexeme_type() == glexemetype::LEXEME_REGULAR_EXPRESSION ? xtokentype::TOKEN_REGULAR_EXPRESSION : xtokentype::TOKEN_LITERAL;

            tokens.push_back(xtoken(token_type, line, column, &symbol, symbol.lexeme));
        }
    }

    // clang-format off
    /*debug*/ m_log.htrace(h, "tokens:") << "\n";
    /*debug*/ for (auto t: tokens)
    /*debug*/     t.json(0, false, 0, false);
    // clang-format on

    m_lexer->compile(tokens);
    m_parser_state_machine->lexer_state_machine = m_lexer->state_machine().get();
}

void gcompiler::populate_whitespace_lexer_state_machine() {
    /*debug*/ std::string h = m_log.hook("build_blanks_scanner_au");

    std::unique_ptr<xcompiler> whitespace_lexer_allocations;

    const std::vector<xtoken> &whitespace_tokens = m_grammar->whitespace_tokens();

    if (!whitespace_tokens.empty()) {
        m_whitespace_lexer->compile(whitespace_tokens);
        m_parser_state_machine->whitespace_lexer_state_machine = m_whitespace_lexer->state_machine().get();
    }
}
