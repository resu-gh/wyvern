#include "include/gcompiler.hpp"
#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gparser.hpp"
#include "include/paction.hpp"
#include "include/pstate.hpp"
#include "include/ptransition.hpp"

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

    const std::vector<std::shared_ptr<gaction>> &grammar_actions = m_generator->actions();

    int actions_size = int(grammar_actions.size());
    std::shared_ptr<std::vector<paction>> actions = std::make_shared<std::vector<paction>>(actions_size);
    assert(actions_size == int(actions->size()));

    for (int i = 0; i < actions_size; ++i) {
        const std::shared_ptr<gaction> &grammar_action = grammar_actions[i];
        assert(grammar_action.get());

        paction &action = actions.get()->at(i);
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

    // /*debug*/ m_log.htrace(h, "computing ggenerator.symbols -> psymbols") << "\n";
    //
    // const std::vector<std::shared_ptr<gsymbol>> &grammar_symbols = m_generator->symbols();
    // // clang-format off
    // /*debug*/ for (auto s : grammar_symbols)
    // /*debug*/     s->json(0, false, 0, true, s.use_count());
    // // clang-format on
    //
    // int symbols_size = grammar_symbols.size();
    // std::shared_ptr<std::vector<psymbol>> symbols = std::make_shared<std::vector<psymbol>>(symbols_size);
    // /*debug*/ m_log.out << m_log.op("symbols_size (cmp): ");
    // /*debug*/ m_log.out << m_log.chl << symbols_size << " == ";
    // /*debug*/ m_log.out << m_log.chl << symbols->size() << " (symbols) \n";
    //
    // assert(symbols_size == int(symbols->size()));
    //
    // for (int i = 0; i < symbols_size; ++i) {
    //     const std::shared_ptr<gsymbol> &source_symbol = grammar_symbols[i];
    //     assert(source_symbol.get());
    //
    //     psymbol &symbol = symbols.get()->at(i);
    //     symbol.index = source_symbol->index();
    //     symbol.identifier = add_string(source_symbol->identifier());
    //     symbol.lexeme = add_string(source_symbol->lexeme());
    //     symbol.type = source_symbol->symbol_type();
    // }
    //
    // // clang-format off
    // /*debug*/ for (auto s : *symbols) {
    // /*debug*/     s.json(0, false, 0, true);
    // /*debug*/ }
    // // clang-format on
}
