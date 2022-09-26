#include "include/xnode.hpp"
#include "include/xtoken.hpp"
#include <algorithm>
#include <cassert>
#include <memory>
#include <string>

xnode::xnode(int index, xnodetype type)
    : m_index(index),
      m_type(type),
      m_begin_char(xnode::INVALID_BEGIN_CHARACTER),
      m_end_char(xnode::INVALID_END_CHARACTER),
      m_nodes(),
      m_nullable(false),
      m_token(),
      m_log("yyv", "xnode", 255) {
    assert(m_type != xnodetype::LEXER_NODE_SYMBOL);
}

xnode::xnode(int index, int begin_char, int end_char)
    : m_index(index),
      m_type(xnodetype::LEXER_NODE_SYMBOL),
      m_begin_char(begin_char),
      m_end_char(end_char),
      m_nodes(),
      m_nullable(false),
      m_token(),
      m_log("yyv", "xnode", 255) {
    assert(m_begin_char < m_end_char);
}

xnode::xnode(int index, int begin_char, int end_char, const std::shared_ptr<xtoken> &token)
    : m_index(index),
      m_type(xnodetype::LEXER_NODE_SYMBOL),
      m_begin_char(begin_char),
      m_end_char(end_char),
      m_nodes(),
      m_nullable(false),
      m_token(token),
      m_log("yyv", "xnode", 255) {
    assert(m_begin_char < m_end_char);
    assert(token.get());
}

void xnode::add_node(const std::shared_ptr<xnode> &node) {
    assert(node.get());
    assert(std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end());
    m_nodes.push_back(node);
}

void xnode::json(int sc, bool nested, int in, bool inlined, int uc) const {
    m_log.out << m_log.chl << m_log.sp(in) << "xnode";
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

    if (!inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "begin: ";
        m_log.out << m_log.chl << m_begin_char;
        m_log.out << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "end: ";
        m_log.out << m_log.chl << m_end_char;
        m_log.out << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc + 2) << "nullable: ";
        m_log.out << m_log.chl << (m_nullable ? "true" : "false");
        m_log.out << m_log.cnr << (inlined ? ", " : ",\n");
    } else {
        m_log.out << m_log.cnr << m_log.sp(0) << "interval: [";
        m_log.out << m_log.chl << m_begin_char;
        m_log.out << m_log.cnr << ", ";
        m_log.out << m_log.chl << m_end_char;
        m_log.out << m_log.cnr << "), ";
    }

    if (!nested && !inlined) {
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "token: ";
        if (m_token)
            m_token->json(sc + 2, true, 0, true, m_token.use_count());
        else
            m_log.out << m_log.chl << "nullptr" << m_log.cnr << ",\n";

        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "childs: [";
        m_log.out << (m_nodes.size() ? "\n" : "");
        for (auto n : m_nodes)
            n->json(sc + 4, true, sc + 4, true, n.use_count());
        m_log.out << m_log.cnr << m_log.sp(sc + 2) << "],\n";
    }

    m_log.out << m_log.cnr << m_log.sp(inlined ? 0 : sc) << "},\n";
}