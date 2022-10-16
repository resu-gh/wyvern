#include "include/xgenerator.hpp"
#include "include/xsyntaxtree.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

xgenerator::xgenerator()
    : m_sytax_tree(std::make_shared<xsyntaxtree>()),
      m_actions(),
      m_log("yyv", "xgenr", 255) {}

std::shared_ptr<xgenerator> xgenerator::self() {
    return shared_from_this();
}
int xgenerator::generate(const std::vector<std::shared_ptr<xtoken>> &tokens) {
    /*debug*/ auto h = m_log.hook("generate");
    /*debug*/ m_log.htrace(h, "generator self: ") << self() << "\n";

    m_sytax_tree->reset(tokens, self());
    /*debug*/ m_log.htrace(h, "xsyntaxtree dump") << "\n";
    m_sytax_tree->dump();
    return 0;
}

// TODO FIXME custom impl
const std::shared_ptr<xaction> &xgenerator::add_lexer_action(const std::string &identifier) {
    assert(!identifier.empty());

    std::vector<std::shared_ptr<xaction>>::const_iterator i = m_actions.begin();
    while (i != m_actions.end() && i->get()->identifier() != identifier)
        ++i;
    if (i == m_actions.end()) {
        std::shared_ptr<xaction> action = std::make_shared<xaction>(int(m_actions.size()), identifier); // TODO cast maybe useless
        m_actions.push_back(action);
        i = m_actions.end() - 1;
    }

    return *i; // TODO maybe m_actions.back()
}
