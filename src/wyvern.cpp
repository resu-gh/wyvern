#include "include/mlogger.hpp"
#include "include/mparser.hpp"
#include "include/mstream.hpp"
#include "lib/include/gcompiler.hpp"

int main(int argc, char *argv[]) {
    mparser mp(argc, argv);
    mstream ms(mp.ifile(), mp.ofile());

    gcompiler compiler;
    std::string::iterator begin = ms.source.begin();
    std::string::iterator end = ms.source.end();
    printf("main [b, e)  = [%p, %p)\n", &*begin, &*end);

    int errors = compiler.compile(begin, end);

    if (errors != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
