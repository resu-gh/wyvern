#include "../lib/ycc/include/yparser.hpp"
#include "../lib/ycc/include/ystream.hpp"
#include "../lib/yv/include/gcompiler.hpp"

int main(int argc, char *argv[]) {
    yparser mp(argc, argv);
    ystream ms(mp.ifile(), mp.ofile());

    gcompiler compiler;
    std::string::iterator begin = ms.source.begin();
    std::string::iterator end = ms.source.end();

    ylogger ylog;
    ylog.out << ylog.cmain;
    ylog.out << "ycc::main = ";
    ylog.out << "[b, e) = ";
    ylog.out << "[" << (void *)&*begin;
    ylog.out << ", " << (void *)&*end << ")\n";
    ylog.out << ylog.creset;

    int errors = compiler.compile(begin, end);

    if (errors != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
