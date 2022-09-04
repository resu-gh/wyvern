#include "include/mlogger.hpp"
#include "include/mparser.hpp"
#include "include/mstream.hpp"
#include "lib/gcompiler.hpp"

int main(int argc, char *argv[]) {
    mparser mp(argc, argv);
    mstream ms(mp.ifile(), mp.ofile());

    gcompiler gc;
    printf("main      [b, e)  = [%p, %p)\n",&*ms.source.begin(), &*ms.source.end());
    int e = gc.compile(ms.source.begin(), ms.source.end());

    if (e != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
