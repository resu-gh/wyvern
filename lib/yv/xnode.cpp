#include "include/xnode.hpp"
#include "include/xaction.hpp"
#include "include/xtoken.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

xnode::xnode(int index, xnodetype type)
    : m_index(index),
      m_type(type),
      m_begin_char(xnode::INVALID_BEGIN_CHARACTER),
      m_end_char(xnode::INVALID_END_CHARACTER),
      m_nodes(),
      m_nullable(false),
      m_token(),
      m_action(),
      m_first_positions(),
      m_last_positions(),
      m_follow_positions(),
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
      m_action(),
      m_first_positions(),
      m_last_positions(),
      m_follow_positions(),
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
      m_action(),
      m_first_positions(),
      m_last_positions(),
      m_follow_positions(),
      m_log("yyv", "xnode", 255) {
    assert(m_begin_char < m_end_char);
    assert(token.get());
}

xnode::xnode(int index, const std::shared_ptr<xaction> &action)
    : m_index(index),
      m_type(xnodetype::LEXER_NODE_ACTION),
      m_begin_char(xnode::BEGIN_CHARACTER),
      m_end_char(xnode::END_CHARACTER),
      m_token(),
      m_action(action),
      m_first_positions(),
      m_last_positions(),
      m_follow_positions(),
      m_nodes(),
      m_nullable(false),
      m_log("yyv", "xnode", 255) {
    assert(m_action.get());
}

std::shared_ptr<xnode> xnode::self() {
    return shared_from_this();
}

int xnode::index() const {
    return m_index;
}

xnodetype xnode::type() const {
    return m_type;
}

bool xnode::is_nullable() const {
    return m_nullable;
}

// TODO FIXME custom impl
std::string xnode::lexeme() const {
    switch (m_type) {
    case xnodetype::LEXER_NODE_NULL:
        return "null";
    case xnodetype::LEXER_NODE_CAT:
        return "cat";
    case xnodetype::LEXER_NODE_OR:
        return "or";
    case xnodetype::LEXER_NODE_STAR:
        return "*";
    case xnodetype::LEXER_NODE_PLUS:
        return "+";
    case xnodetype::LEXER_NODE_OPTIONAL:
        return ">";
    case xnodetype::LEXER_NODE_SYMBOL:
        return ".";
    case xnodetype::LEXER_NODE_ACTION:
        return "action";
    default:
        assert(false);
        break;
    }
}

void xnode::add_node(const std::shared_ptr<xnode> &node) {
    assert(node.get());
    assert(std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end());
    m_nodes.push_back(node);
}

void xnode::calculate_nullable() {
    using node_iter = std::vector<std::shared_ptr<xnode>>::const_iterator;
    for (node_iter i = m_nodes.begin(); i != m_nodes.end(); ++i) {
        assert(i->get());
        const std::shared_ptr<xnode> &node = *i;
        node->calculate_nullable();
    }
    switch (m_type) {
    case xnodetype::LEXER_NODE_NULL:
        m_nullable = true;
        break;
    case xnodetype::LEXER_NODE_CAT:
        assert(m_nodes.size() == 2);
        m_nullable = m_nodes[0]->is_nullable() && m_nodes[1]->is_nullable();
        break;
    case xnodetype::LEXER_NODE_OR:
        assert(m_nodes.size() == 2);
        m_nullable = m_nodes[0]->is_nullable() || m_nodes[1]->is_nullable();
        break;
    case xnodetype::LEXER_NODE_STAR:
    case xnodetype::LEXER_NODE_OPTIONAL:
        m_nullable = true;
        break;
    case xnodetype::LEXER_NODE_PLUS:
        assert(m_nodes.size() == 1);
        m_nullable = m_nodes[0]->is_nullable();
        break;
    case xnodetype::LEXER_NODE_SYMBOL:
        m_nullable = m_begin_char == xnode::INVALID_BEGIN_CHARACTER && m_end_char == xnode::INVALID_END_CHARACTER;
        break;
    case xnodetype::LEXER_NODE_ACTION:
        m_nullable = false;
        break;
    default:
        assert(false);
        m_nullable = false;
        break;
    }
}

void xnode::calculate_first_positions() {
    using node_iter = std::vector<std::shared_ptr<xnode>>::const_iterator;
    for (node_iter i = m_nodes.begin(); i != m_nodes.end(); ++i) {
        assert(i->get());
        const std::shared_ptr<xnode> &node = *i;
        node->calculate_first_positions();
    }
    switch (m_type) {
    case xnodetype::LEXER_NODE_NULL:
        break;
    case xnodetype::LEXER_NODE_CAT: {
        assert(m_nodes.size() == 2);
        m_first_positions.insert(m_nodes[0]->m_first_positions.begin(), m_nodes[0]->m_first_positions.end());
        if (m_nodes[0]->is_nullable())
            m_first_positions.insert(m_nodes[1]->m_first_positions.begin(), m_nodes[1]->m_first_positions.end());
        break;
    }
    case xnodetype::LEXER_NODE_OR:
        assert(m_nodes.size() == 2);
        m_first_positions.insert(m_nodes[0]->m_first_positions.begin(), m_nodes[0]->m_first_positions.end());
        m_first_positions.insert(m_nodes[1]->m_first_positions.begin(), m_nodes[1]->m_first_positions.end());
        break;
    case xnodetype::LEXER_NODE_STAR:
    case xnodetype::LEXER_NODE_PLUS:
    case xnodetype::LEXER_NODE_OPTIONAL:
        assert(m_nodes.size() == 1);
        m_first_positions.insert(m_nodes[0]->m_first_positions.begin(), m_nodes[0]->m_first_positions.end());
        break;
    case xnodetype::LEXER_NODE_SYMBOL:
    case xnodetype::LEXER_NODE_ACTION:
        m_first_positions.insert(self());
        break;
    default:
        assert(false);
        break;
    }
}

void xnode::calculate_last_positions() {
    using node_iter = std::vector<std::shared_ptr<xnode>>::const_iterator;
    for (node_iter i = m_nodes.begin(); i != m_nodes.end(); ++i) {
        assert(i->get());
        const std::shared_ptr<xnode> &node = *i;
        node->calculate_last_positions();
    }
    switch (m_type) {
    case xnodetype::LEXER_NODE_NULL:
        break;
    case xnodetype::LEXER_NODE_CAT: {
        assert(m_nodes.size() == 2);
        m_last_positions.insert(m_nodes[1]->m_last_positions.begin(), m_nodes[1]->m_last_positions.end());
        if (m_nodes[1]->is_nullable())
            m_last_positions.insert(m_nodes[0]->m_last_positions.begin(), m_nodes[0]->m_last_positions.end());
        break;
    }
    case xnodetype::LEXER_NODE_OR:
        assert(m_nodes.size() == 2);
        m_last_positions.insert(m_nodes[0]->m_last_positions.begin(), m_nodes[0]->m_last_positions.end());
        m_last_positions.insert(m_nodes[1]->m_last_positions.begin(), m_nodes[1]->m_last_positions.end());
        break;
    case xnodetype::LEXER_NODE_STAR:
    case xnodetype::LEXER_NODE_PLUS:
    case xnodetype::LEXER_NODE_OPTIONAL:
        assert(m_nodes.size() == 1);
        m_last_positions.insert(m_nodes[0]->m_last_positions.begin(), m_nodes[0]->m_last_positions.end());
        break;
    case xnodetype::LEXER_NODE_SYMBOL:
    case xnodetype::LEXER_NODE_ACTION:
        m_last_positions.insert(self());
        break;
    default:
        assert(false);
        break;
    }
}

void xnode::calculate_follow_positions() {
    using node_iter = std::vector<std::shared_ptr<xnode>>::const_iterator;
    for (node_iter i = m_nodes.begin(); i != m_nodes.end(); ++i) {
        assert(i->get());
        const std::shared_ptr<xnode> &node = *i;
        node->calculate_follow_positions();
    }
    switch (m_type) {
    case xnodetype::LEXER_NODE_NULL:
        break;
    case xnodetype::LEXER_NODE_CAT: {
        const std::set<std::shared_ptr<xnode>, xnodec> &last_positions = m_nodes[0]->m_last_positions;
        const std::set<std::shared_ptr<xnode>, xnodec> &first_positions = m_nodes[1]->m_first_positions;
        using nodeset_iter = std::set<std::shared_ptr<xnode>, xnodec>::const_iterator;
        for (nodeset_iter i = last_positions.begin(); i != last_positions.end(); ++i) {
            assert(i->get());
            const std::shared_ptr<xnode> &node = *i;
            if (node->type() == xnodetype::LEXER_NODE_SYMBOL || node->type() == xnodetype::LEXER_NODE_ACTION)
                node->m_follow_positions.insert(first_positions.begin(), first_positions.end());
        }
        break;
    }
    case xnodetype::LEXER_NODE_STAR:
    case xnodetype::LEXER_NODE_PLUS: {
        assert(m_nodes.size() == 1);
        const std::set<std::shared_ptr<xnode>, xnodec> &last_positions = m_last_positions;
        const std::set<std::shared_ptr<xnode>, xnodec> &first_positions = m_first_positions;
        using nodeset_iter = std::set<std::shared_ptr<xnode>, xnodec>::const_iterator;
        for (nodeset_iter i = last_positions.begin(); i != last_positions.end(); ++i) {
            assert(i->get());
            const std::shared_ptr<xnode> &node = *i;
            if (node->type() == xnodetype::LEXER_NODE_SYMBOL || node->type() == xnodetype::LEXER_NODE_ACTION)
                node->m_follow_positions.insert(first_positions.begin(), first_positions.end());
        }
        break;
    }
    case xnodetype::LEXER_NODE_OR:
    case xnodetype::LEXER_NODE_OPTIONAL:
    case xnodetype::LEXER_NODE_SYMBOL:
    case xnodetype::LEXER_NODE_ACTION:
        break;
    default:
        assert(false);
        break;
    }
}

bool xnode::operator<(const xnode &node) const {
    return m_type < node.m_type || (m_type == node.m_type && m_index < node.m_index);
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
