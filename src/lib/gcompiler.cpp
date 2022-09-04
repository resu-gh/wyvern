#include "lib/include/gcompiler.hpp"

#include <iostream>

gcompiler::gcompiler()
    : m_grammar(std::make_shared<ggrammar>()) {}

int gcompiler::compile(std::string::iterator &begin, std::string::iterator &end) {
    printf("gcom [b, e)  = [%p, %p)\n", &*begin, &*end);
    printf("gcom grammar = %p %ld\n", &*m_grammar, m_grammar.use_count());

    gparser parser;
    int errors = parser.parse(begin, end, m_grammar);

    if (errors == 0) {
        ggenerator generator;
        errors = generator.generate(m_grammar);

        if (errors == 0) {
            printf("gcom ok\n");
        }
    }

    return errors;
}
