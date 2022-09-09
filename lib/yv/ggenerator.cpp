#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"

#include <iostream>

ggenerator::ggenerator() {}

int ggenerator::generate(const std::shared_ptr<ggrammar> &grammar) {
    m_grammar = grammar;

    /*debug*/ m_log.out << m_log.cggenr << "yv::ggenr = ";
    /*debug*/ m_log.out << "&ggrammar = ";
    /*debug*/ m_log.out << (void *)&*m_grammar << " ";
    /*debug*/ m_log.out << "[uses: " << m_grammar.use_count() << "]";
    /*debug*/ m_log.out << m_log.creset << "\n";

    m_grammar->dump();

    return 0;
}
