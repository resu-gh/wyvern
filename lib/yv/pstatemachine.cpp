#include "include/pstatemachine.hpp"

pstatemachine::pstatemachine()
    : m_log("yyv", "pstma", 255) {}

void pstatemachine::json(int sc, bool nested, int in, bool inlined, int uc) const {
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

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "identifier: ";
    m_log.out << m_log.chl << identifier;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    // if (!inlined) {
    //     m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "start_symbol: ";
    //     start_symbol->json(sc + 2, false, sc + 2, true, start_symbol.use_count());
    //
    //     m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "end_symbol: ";
    //     end_symbol->json(sc + 2, false, sc + 2, true, end_symbol.use_count());
    //
    //     m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "error_symbol: ";
    //     error_symbol->json(sc + 2, false, sc + 2, true, error_symbol.use_count());
    // }

    if (!nested && !inlined) {
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "actions: [\n";
        // for (auto a = actions_begin; a != actions_end; ++a)
        //     a->json(sc + 4, true, sc + 4, true);
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";

        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "symbols: [\n";
        // for (auto s = symbols_begin; s != symbols_end; ++s)
        //     s->json(sc + 4, true, sc + 4, true);
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";

        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "transitions: [\n";
        // for (auto t = transitions_begin; t != transitions_end; ++t)
        //     t->json(sc + 4, true, sc + 4, true);
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";

        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "states: [\n";
        // for (auto s = states_begin; s != states_end; ++s)
        //     s->json(sc + 4, true, sc + 4, true);
        // m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
