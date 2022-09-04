#include "include/mlogger.hpp"
#include "include/mparser.hpp"
#include "include/mstream.hpp"
#include <iterator>
#include <string>
#include <cstring>

int main(int argc, char *argv[]) {
    mparser mp(argc, argv);
    mstream ms(mp.ifile(), mp.ofile());

    mlogger ml;
    ml.out << ml.cred << ms.source << ml.creset << "\n";
    std::string::const_iterator b = ms.source.begin();
    std::string::const_iterator e = ms.source.end();
    const char *ab = &*b;
    const char *ae = &*e;
    ml.out << "[" << &ab << ", " << &ae << ")\n";
    ml.out << "[" << *ab << ", " << *ae << ")";

    ml.out << (*ae == '\0' ? "true" : "false") << "\n";
    ml.out << std::distance(ab, ae) << "\n";
    ml.out << ms.source.size() << "\n";
    ml.out << strlen(ms.source.c_str()) << "\n";

    return 0;
}
