#include "include/xtoken.hpp"

xtoken::xtoken(xtokentype type, int line, int column, const std::shared_ptr<void> symbol, const std::string &lexeme)
    : m_type(type),
      m_line(line),
      m_column(column),
      m_symbol(symbol),
      m_lexeme(lexeme),
      m_conflicted_with(),
      m_log("yyv", "xtokn", 255) {}

xtokentype xtoken::type() const {
    return m_type;
}

int xtoken::line() const {
    return m_line;
}

int xtoken::column() const {
    return m_column;
}

const std::shared_ptr<void> &xtoken::symbol() const {
    return m_symbol;
}

const std::string &xtoken::lexeme() const {
    return m_lexeme;
}

void xtoken::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "xtokn";
    m_log.out << m_log.cnr << (inlined ? ": { " : ": {\n");

    if (uc) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "use_count: ";
        m_log.out << m_log.chl << uc;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "this: ";
    m_log.out << m_log.chl << &*this;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "type: ";
    m_log.out << m_log.chl << m_type;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "lexeme: ";
    m_log.out << m_log.chl << m_lexeme;
    m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

    if (!inlined) {
        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "line: ";
        m_log.out << m_log.chl << m_line;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "column: ";
        m_log.out << m_log.chl << m_column;
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    }

    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "conflicted_with: [";
        m_log.out << (m_conflicted_with.size() ? "\n" : "");
        for (auto c : m_conflicted_with)
            c->json(sc + 4, true, sc + 4, true, c.use_count());
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "]\n";
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}
