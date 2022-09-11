#include "include/gcompiler.hpp"
#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"
#include "include/gparser.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

gcompiler::gcompiler()
    : m_grammar(std::make_shared<ggrammar>()),
      m_log("yyv", "gcomp", 45) {}

int gcompiler::compile(std::string::iterator &begin, std::string::iterator &end) {
    m_log.set_fun("compile");

    /*debug*/ m_log.trace(0) << m_log.op("get") << "[begin, end) ";
    /*debug*/ m_log.out << m_log.chl << "[" << (void *)&*begin << ", ";
    /*debug*/ m_log.out << (void *)&*end << ")\n";
    /*debug*/ m_log.trace(1) << m_log.op("get") << "m_ggrammar ";
    /*debug*/ m_log.out << m_log.chl << (void *)&*m_grammar << " ";
    /*debug*/ m_log.out << "<" << m_grammar.use_count() << ">\n";

    gparser parser;
    int errors = parser.parse(begin, end, m_grammar);

    if (errors == 0) {
        ggenerator generator;
        errors = generator.generate(m_grammar);

        if (errors == 0) {
            /*debug*/ m_log.trace(0) << m_log.op("check");
            /*debug*/ m_log.out << m_log.chl << "parse && generation: OK\n";
        }
    }

    return errors;
}
