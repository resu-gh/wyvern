#include "include/gcompiler.hpp"
#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gparser.hpp"
#include "include/paction.hpp"

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

            // /*debug*/ m_log.htrace(h, "populating lexer state machine") << "\n";

            // /*debug*/ m_log.htrace(h, "populating whitespace lexer state machine") << "\n";
        }
    }

    return errors;
}

const std::string &gcompiler::add_string(const std::string &string) {
    m_strings.push_back(string);
    return m_strings.back();
}

void gcompiler::populate_parser_state_machine() {
    /*debug*/ std::string h = m_log.hook("build_parser_au");

    /*debug*/ m_log.htrace(h, "WIP!") << "\n";

    //! test

    const std::vector<std::shared_ptr<gaction>> &grammar_actions = m_generator->actions();
    // clang-format off
    /*debug*/ for (auto a : grammar_actions)
    /*debug*/     a->json(0, false, 0, true, a.use_count());
    // clang-format on

    int actions_size = int(grammar_actions.size());
    m_actions = std::make_unique<std::vector<paction>>(actions_size);
    /*debug*/ m_log.out << m_log.op("actions_size (cmp): ");
    /*debug*/ m_log.out << m_log.chl << actions_size << " == ";
    /*debug*/ m_log.out << m_log.chl << m_actions->size() << " (actions) \n";

    assert(actions_size == m_actions->size());

    for (int i = 0; i < actions_size; ++i) {
        const std::shared_ptr<gaction> &grammar_action = grammar_actions[i];
        assert(grammar_action.get());

        paction &action = m_actions.get()->at(i);
        action.index = grammar_action->index();
        action.identifier = add_string(grammar_action->identifier());
    }
    // clang-format off
    /*debug*/ for (auto a : *m_actions)
    /*debug*/     m_log.out << a.index << " " << a.identifier << "\n";
    // clang-format on

    // --------------------------------------------------

    const std::vector<std::shared_ptr<gsymbol>> &grammar_symbols = m_generator->symbols();
    // clang-format off
    /*debug*/ for (auto s : grammar_symbols)
    /*debug*/     s->json(0, false, 0, true, s.use_count());
    // clang-format on

    int symbols_size = grammar_symbols.size();
    m_symbols = std::make_unique<std::vector<psymbol>>(symbols_size);
    /*debug*/ m_log.out << m_log.op("symbols_size (cmp): ");
    /*debug*/ m_log.out << m_log.chl << symbols_size << " == ";
    /*debug*/ m_log.out << m_log.chl << m_symbols->size() << " (symbols) \n";

    assert(symbols_size == m_symbols->size());

    for (int i = 0; i < symbols_size; ++i) {
        const std::shared_ptr<gsymbol> &source_symbol = grammar_symbols[i];
        assert(source_symbol.get());

        psymbol &symbol = m_symbols.get()->at(i);
        symbol.index = source_symbol->index();
        symbol.identifier = add_string(source_symbol->identifier());
        symbol.lexeme = add_string(source_symbol->lexeme());
        symbol.type = source_symbol->symbol_type();
    }

    // clang-format off
    /*debug*/ for (auto s : *m_symbols) {
    /*debug*/     m_log.out << s.index << " " << s.identifier << " ";
    /*debug*/     m_log.out << s.lexeme << " " << s.type << "\n";
    /*debug*/ }
    // clang-format on

    // --------------------------------------------------

    const std::set<std::shared_ptr<gstate>, gstatec> &grammar_states = m_generator->states();
    // clang-format off
    /*debug*/ for (auto s : grammar_states)
    /*debug*/     s->json(0, false, 0, true, s.use_count());
    // clang-format on

    int states_size = int(grammar_states.size());
    m_states = std::make_unique<std::vector<pstate>>(states_size);
    /*debug*/ m_log.out << m_log.op("states_size (cmp): ");
    /*debug*/ m_log.out << m_log.chl << states_size << " == ";
    /*debug*/ m_log.out << m_log.chl << m_states->size() << " (states) \n";

    assert(states_size == m_states->size());

    //! test 2
    int transitions_size = 0;
    using state_iter = std::set<std::shared_ptr<gstate>, gstatec>::iterator;
    for (state_iter i = grammar_states.begin(); i != grammar_states.end(); ++i) {
        std::shared_ptr<gstate> grammar_state = *i;
        assert(grammar_state);

        transitions_size += int(grammar_state->transitions().size());
    }
    std::cout << transitions_size << "\n";
    //! test 2

    // clang-format off
    /*debug*/ for (auto s : *m_states) {
    /*debug*/     m_log.out << s.index << " " << s.length << "\n";
    /*debug*/ }
    // clang-format on

    //! test
}