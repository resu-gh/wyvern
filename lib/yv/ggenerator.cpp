#include "include/ggenerator.hpp"
#include "include/ggrammar.hpp"

#include <iostream>

ggenerator::ggenerator() {}

int ggenerator::generate(const std::shared_ptr<ggrammar> &grammar) {
    m_grammar = grammar;

    // debug begin
    m_log.out << m_log.cggenr << "yv::ggenr = ";
    m_log.out << "&ggrammar = ";
    m_log.out << (void *)&*m_grammar << " ";
    m_log.out << "[uses: " << m_grammar.use_count() << "]";
    m_log.out << m_log.creset << "\n";

    m_log.out << m_log.cggenr << "yv::ggenr = ";
    m_log.out << "ggrammar ident = ";
    m_log.out << m_grammar->identifier();
    m_log.out << m_log.creset << "\n";

    /// convert into m_grammar.dump()
    m_log.out << m_log.cggenr;
    for (auto s : m_grammar->symbols())
        m_log.out << "                             " << s->lexeme() << "\n";
    m_log.out << m_log.creset;
    // debug end

    return 0;
}
