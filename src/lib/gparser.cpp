#include "lib/gparser.hpp"

gparser::gparser() {}

int gparser::parse(const std::string::const_iterator b, const std::string::const_iterator e, ggrammar *g) {
    printf("gparser   [b, e)  = [%p, %p)\n", &*b, &*e);
    printf("gcompiler grammar = %p\n", &*g);
    return 0;
}
