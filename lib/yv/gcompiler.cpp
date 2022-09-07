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
    // debug begin
    m_log.out << m_log.cgcomp << "yv::gcomp = ";
    m_log.out << "[b, e) = ";
    m_log.out << "[" << (void *)&*begin;
    m_log.out << ", " << (void *)&*end << ")";
    m_log.out << m_log.creset << "\n";

    m_log.out << m_log.cgcomp << "yv::gcomp = ";
    m_log.out << "&ggrammar = ";
    m_log.out << (void *)&*m_grammar;
    m_log.out << " [uses: " << m_grammar.use_count() << "]";
    m_log.out << m_log.creset << "\n";
    // debug end

    gparser parser;
    int errors = parser.parse(begin, end, m_grammar);

    // debug begin
    m_log.out << m_log.cgcomp << "yv::gcomp = ";
    m_log.out << "ggrammar ident = ";
    m_log.out << m_grammar->identifier();
    m_log.out << m_log.creset << "\n";
    // debug end

    if (errors == 0) {
        ggenerator generator;
        errors = generator.generate(m_grammar);

        if (errors == 0) {
            printf("gcom ok\n");
        }
    }

    return errors;
}
