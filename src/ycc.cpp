#include "../lib/ycc/include/yparser.hpp"
#include "../lib/ycc/include/ystream.hpp"
#include "../lib/yv/include/gcompiler.hpp"

int main(int argc, char *argv[]) {
    yparser mp(argc, argv);
    ystream ms(mp.ifile(), mp.ofile());

    gcompiler compiler;
    std::string::iterator begin = ms.source.begin();
    std::string::iterator end = ms.source.end();

    /*debug*/ ylogger ylog;
    /*debug*/ ylog.out << ylog.cmain;
    /*debug*/ ylog.out << "ycc:main:main()             ";
    /*debug*/ ylog.out << ylog.cmagenta << "get ";
    /*debug*/ ylog.out << "\033[38;5;240m";
    /*debug*/ ylog.out << "[begin, end) ";
    /*debug*/ ylog.out << "\033[38;5;245m";
    /*debug*/ ylog.out << "[" << (void *)&*begin;
    /*debug*/ ylog.out << ", " << (void *)&*end << ")\n";
    /*debug*/ ylog.out << ylog.creset;

    int errors = compiler.compile(begin, end);

    if (errors != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
