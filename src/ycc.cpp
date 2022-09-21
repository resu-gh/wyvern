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

    return EXIT_SUCCESS;
}
