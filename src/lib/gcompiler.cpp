#include "lib/gcompiler.hpp"

gcompiler::gcompiler() {}

int gcompiler::compile(const std::string::const_iterator &b, const std::string::const_iterator &e) {
    printf("gcompiler [b, e)  = [%p, %p)\n", &*b, &*e);

    ggrammar grammar;
    printf("gcompiler grammar = %p\n", &grammar);

    gparser parser;
    int err = parser.parse(b, e, &grammar);

    return err;
}
