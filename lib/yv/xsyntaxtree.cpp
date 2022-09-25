#include "include/xsyntaxtree.hpp"
#include "include/xgenerator.hpp"
#include <cassert>

xsyntaxtree::xsyntaxtree()
    : m_generator(),
      m_log("yyv", "xtree", 255) {}

void xsyntaxtree::reset(const std::vector<xtoken> &tokens, const std::shared_ptr<xgenerator> &generator) {
    /*debug*/ std::string h = m_log.hook("reset");
    /*debug*/ m_log.htrace(h, "generator (param)") << &*generator << "\n";

    assert(generator);
    m_generator = generator;
    /*debug*/ m_log.htrace(h, "m_generator") << &*m_generator << "\n";

    calculate_combined_parse_tree(tokens);

    exit(0);
}

void xsyntaxtree::calculate_combined_parse_tree(const std::vector<xtoken> &tokens) {
    /*debug*/ std::string h = m_log.hook("cmp_combo_tree");

    /*debug*/ m_log.htrace(h, "iterate tokens") << "\n";
    using token_iter = std::vector<xtoken>::const_iterator;
    for (token_iter token = tokens.begin(); token != tokens.end(); ++token) {
        /*debug*/ m_log.htrace(h, "processing token");
        /*debug*/ token->json(0, false, 0, true);

        switch (token->type()) {
        case xtokentype::TOKEN_REGULAR_EXPRESSION:
            parse_regular_expression(*token);
            break;
        case xtokentype::TOKEN_LITERAL:
            parse_literal(*token);
            break;
        case xtokentype::TOKEN_NULL:
            assert(false);
            break;
        }
    }
}

void xsyntaxtree::parse_regular_expression(const xtoken &token) {
    /*debug*/ std::string h = m_log.hook("parse_regex");
    /*debug*/ m_log.htrace(h, "regex token");
    /*debug*/ token.json(0, false, 0, true);
}

void xsyntaxtree::parse_literal(const xtoken &token) {
    /*debug*/ std::string h = m_log.hook("parse_regex");
    /*debug*/ m_log.htrace(h, "literal token");
    /*debug*/ token.json(0, false, 0, true);
}
