#include "include/pstate.hpp"
#include "include/ptransition.hpp"

pstate::pstate()
    : m_log("yyv", "pstat", 255) {}

void pstate::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "pstat";
    m_log.out << m_log.cnr << (inlined ? ": { " : ": {\n");

    if (uc) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "use_count: ";
        m_log.out << m_log.chl << uc;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "this: ";
    m_log.out << m_log.chl << &*this;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "index: ";
    m_log.out << m_log.chl << index;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "length: ";
    m_log.out << m_log.chl << length;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    if (!nested && !inlined) {
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "transition: [";
        // m_log.out << (transitions.size() ? "\n" : "");
        // for (auto t : transitions)
        //     t->json(sc + 4, true, sc + 4, true);
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "transitions: ";
        if (transitions.get())
            transitions->json(0, true, 0, true, transitions.use_count());
        else
            m_log.out << m_log.chl << "nullptr";
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
