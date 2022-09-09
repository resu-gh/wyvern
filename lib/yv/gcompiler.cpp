#include "include/gcompiler.hpp"
#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gparser.hpp"

#include <cstdio>
#include <iostream>

gcompiler::gcompiler()
    : m_grammar(std::make_shared<ggrammar>()),
      m_log() {}

int gcompiler::compile(std::string::iterator &begin, std::string::iterator &end) {
    /*debug*/ m_log.out << m_log.cgcomp << "yv::gcomp = ";
    /*debug*/ m_log.out << "[b, e) = ";
    /*debug*/ m_log.out << "[" << (void *)&*begin;
    /*debug*/ m_log.out << ", " << (void *)&*end << ")";
    /*debug*/ m_log.out << m_log.creset << "\n";

    /*debug*/ m_log.out << m_log.cgcomp << "yv::gcomp = ";
    /*debug*/ m_log.out << "&ggrammar = ";
    /*debug*/ m_log.out << (void *)&*m_grammar;
    /*debug*/ m_log.out << " [uses: " << m_grammar.use_count() << "]";
    /*debug*/ m_log.out << m_log.creset << "\n";

    gparser parser;
    int errors = parser.parse(begin, end, m_grammar);

    if (errors == 0) {
        ggenerator generator;
        errors = generator.generate(m_grammar);

        if (errors == 0) {
            printf("gcom no errors\n");
        }
    }

    return errors;
}
