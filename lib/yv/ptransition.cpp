#include "include/ptransition.hpp"
#include "include/pstate.hpp"
#include "include/psymbol.hpp"

ptransition::ptransition()
    : m_log("yyv", "ptran", 255) {}

void ptransition::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "ptran";
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

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "type: ";
    m_log.out << m_log.chl << type;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    if (!inlined) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "precedence: ";
        m_log.out << m_log.chl << precedence;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "action_index: ";
        m_log.out << m_log.chl << action;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "reduced_length: ";
        m_log.out << m_log.chl << reduced_length;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    // recursive begin
    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "state: ";
        if (state)
            state.get()->json(sc + 2, true, 0, true, state.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "symbol: ";
        if (symbol)
            symbol->json(sc + 2, true, 0, true, symbol.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "reduced_symbol: ";
        if (reduced_symbol)
            reduced_symbol->json(sc + 2, true, 0, true, reduced_symbol.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";
    }
    // recursive end

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
