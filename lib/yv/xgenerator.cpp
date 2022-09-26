#include "include/xgenerator.hpp"
#include "include/xsyntaxtree.hpp"

#include <iostream>
#include <memory>

xgenerator::xgenerator()
    : m_sytax_tree(std::make_shared<xsyntaxtree>()),
      m_log("yyv", "xgenr", 255) {}

std::shared_ptr<xgenerator> xgenerator::self() {
    return shared_from_this();
}
int xgenerator::generate(const std::vector<std::shared_ptr<xtoken>> &tokens) {
    /*debug*/ auto h = m_log.hook("generate");
    /*debug*/ m_log.htrace(h, "generator self: ") << self() << "\n";

    m_sytax_tree->reset(tokens, self());
    return 0;
}
