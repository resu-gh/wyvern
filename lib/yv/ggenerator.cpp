#include "include/ggenerator.hpp"

#include <iostream>

ggenerator::ggenerator() {}

int ggenerator::generate(const std::shared_ptr<ggrammar> &grammar) {
    printf("ggen grammar = %p %ld\n", (void *)&*grammar, grammar.use_count());
    return 0;
}
