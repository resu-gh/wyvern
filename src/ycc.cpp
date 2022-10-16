#include "../lib/ycc/include/yparser.hpp"
#include "../lib/ycc/include/ystream.hpp"
#include "../lib/yv/include/gcompiler.hpp"

int main(int argc, char *argv[]) {
    yparser mp(argc, argv);
    ystream ms(mp.ifile(), mp.ofile());

    gcompiler compiler;
    std::string::iterator begin = ms.source.begin();
    std::string::iterator end = ms.source.end();

    /*debug*/ glogger log("ycc", "main", 110);
    /*debug*/ std::string h = log.hook("main");
    /*debug*/ log.htrace(h, "interval (builded)") << "[begin, end) ";
    /*debug*/ log.out << log.chl << "[" << (void *)&*begin;
    /*debug*/ log.out << ", " << (void *)&*end << ")\n";

    int errors = compiler.compile(begin, end);

    if (errors != 0)
        return EXIT_FAILURE;

    const std::unique_ptr<pstatemachine> &automaton = compiler.parser_state_machine();
    /*debug*/ log.htrace(h, "parser state machine") << "\n";
    /*debug*/ automaton->json(0, false, 0, false);

    /*debug*/ log.htrace(h, "writing to output") << log.creset << "\n";
    exit(0);

    // temp
    auto tab = std::string(4, ' '); // add flag -t <integer>

    ms.out << "#include <wyvern/saction.hpp>\n";
    ms.out << "#include <wyvern/sstate.hpp>\n";
    ms.out << "#include <wyvern/sautomaton.hpp>\n";
    ms.out << "#include <wyvern/stransition.hpp>\n";
    ms.out << "#include <wyvern/paction.hpp>\n";
    ms.out << "#include <wyvern/pstate.hpp>\n";
    ms.out << "#include <wyvern/pautomaton.hpp>\n";
    ms.out << "#include <wyvern/psymbol.hpp>\n";
    ms.out << "#include <wyvern/ptransition.hpp>\n\n";

    ms.out << "using namespace wyvern;\n\n";

    ms.out << "extern const paction actions[] = {\n";
    for (auto a = automaton->actions; a != automaton->actions + automaton->actions_size; ++a) {
        ms.out << ms.fmt("%s{ %d, \"%s\" },\n", tab.c_str(), a->index, a->identifier);
    }
    ms.out << ms.fmt("%s{ -1, nullptr },\n};\n\n", tab.c_str());

    ms.out << "extern const psymbol symbols[] = {\n";
    for (auto s = automaton->symbols; s != automaton->symbols + automaton->symbols_size; ++s) {
        ms.out << ms.fmt("%s{ %d, \"%s\", \"%s\", (gsymboltype) %d },\n", tab.c_str(), s->index, s->identifier, ms.sanitize(s->lexeme).c_str(), s->type);
    }
    ms.out << ms.fmt("%s{ -1, nullptr, nullptr, (gsymboltype) 0 },\n};\n\n", tab.c_str());

    ms.out << "extern const ptransition transitions[] = {\n";
    for (auto t = automaton->transitions; t != automaton->transitions + automaton->transitions_size; ++t) {
        if (t->reduced_symbol)
            ms.out << ms.fmt("%s{ &symbols[%d], nullptr, &symbols[%d], %d, %d, %d, (gtranstype) %d, %d },\n",
                             tab.c_str(),
                             t->symbol ? t->symbol->index : -1,
                             t->reduced_symbol->index,
                             t->reduced_length,
                             t->precedence,
                             t->action,
                             t->type,
                             t->index);
        else
            ms.out << ms.fmt("%s{ &symbols[%d], nullptr, &symbols[%d], %d, %d, %d, (gtranstype) %d, %d },\n",
                             tab.c_str(),
                             t->symbol ? t->symbol->index : -1,
                             t->state ? t->state->index : -1,
                             t->reduced_length,
                             t->precedence,
                             t->action,
                             t->type,
                             t->index);
    }
    ms.out << ms.fmt("%s{ nullptr, nullptr, nullptr, 0, 0, 0, (gtranstype) 1, -1 },\n};\n\n", tab.c_str());

    return EXIT_SUCCESS;
}
