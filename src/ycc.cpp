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

    const std::shared_ptr<pstatemachine> &parser_state_machine = compiler.parser_state_machine();
    /*debug*/ log.htrace(h, "parser state machine") << "\n";
    /*debug*/ parser_state_machine->json(0, false, 0, false);

    /*debug*/ log.htrace(h, "writing to output") << log.creset << "\n";

    // temp
    auto tab = std::string(4, ' '); // add flag -t <integer>

    std::array<std::string, 4> gs = {
        "gsymboltype::NONE",
        "gsymboltype::TERM",
        "gsymboltype::NON_TERM",
        "gsymboltype::END",
    };
    std::array<std::string, 2> ts = {
        "gtranstype::SHIFT",
        "gtranstype::REDUCE",
    };
    // temp

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
    for (auto a = parser_state_machine->actions_begin; a != parser_state_machine->actions_end; ++a) {
        ms.out << ms.fmt("%s{ %d, \"%s\" },\n", tab.c_str(), a->index, a->identifier.c_str());
    }
    ms.out << tab << "{ -1, nullptr },\n};\n\n";

    ms.out << "extern const psymbol symbols[] = {\n";
    for (auto s = parser_state_machine->symbols_begin; s != parser_state_machine->symbols_end; ++s) {
        ms.out << ms.fmt("%s{ %d, \"%s\", \"%s\", %s },\n", tab.c_str(), s->index, s->identifier.c_str(), ms.sanitize(s->lexeme).c_str(), gs[static_cast<int>(s->type)].c_str());
    }
    ms.out << tab << "{ -1, nullptr, nullptr, " << gs[0] << " },\n};\n\n";

    ms.out << "extern const ptransition transitions[] = {\n";
    for (auto t = parser_state_machine->transitions_begin; t != parser_state_machine->transitions_end; ++t) {
        if (t->reduced_symbol)
            ms.out << ms.fmt("%s{ &symbols[%d], nullptr, &symbols[%d], %d, %d, %d, %s, %d },\n",
                             tab.c_str(),
                             t->symbol ? t->symbol->index : -1,
                             t->reduced_symbol->index,
                             t->reduced_length,
                             t->precedence,
                             t->action,
                             ts[static_cast<int>(t->type)].c_str(),
                             t->index);
        else
            ms.out << ms.fmt("%s{ &symbols[%d], nullptr, &symbols[%d], %d, %d, %d, %s, %d },\n",
                             tab.c_str(),
                             t->symbol ? t->symbol->index : -1,
                             t->state ? t->state->index : -1,
                             t->reduced_length,
                             t->precedence,
                             t->action,
                             ts[static_cast<int>(t->type)].c_str(),
                             t->index);
    }
    ms.out << tab << "{ nullptr, nullptr, nullptr, 0, 0, 0, " << ts[0] << ", -1 },\n};\n\n";

    return EXIT_SUCCESS;
}
