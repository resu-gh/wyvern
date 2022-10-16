#include "include/xsyntaxtree.hpp"
#include "include/ecode.hpp"
#include "include/xchars.hpp"
#include "include/xgenerator.hpp"
#include "include/xnode.hpp"
#include "include/xnodetype.hpp"
#include "include/xparser.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <utility>

xsyntaxtree::xsyntaxtree()
    : m_generator(),
      m_index(0),
      m_errors(0),
      m_nodes(),
      m_bracket_expression_characters(),
      m_log("yyv", "xtree", 255) {}

std::shared_ptr<xsyntaxtree> xsyntaxtree::self() {
    return shared_from_this();
}

void xsyntaxtree::reset(const std::vector<std::shared_ptr<xtoken>> &tokens, const std::shared_ptr<xgenerator> &generator) {
    /*debug*/ std::string h = m_log.hook("reset");
    /*debug*/ m_log.htrace(h, "generator (param)") << &*generator << "\n";

    assert(generator);
    m_generator = generator;
    /*debug*/ m_log.htrace(h, "m_generator") << &*m_generator << "\n";

    calculate_combined_parse_tree(tokens);
    // calculate_nullable_first_last_and_follow();
}

void xsyntaxtree::calculate_combined_parse_tree(const std::vector<std::shared_ptr<xtoken>> &tokens) {
    /*debug*/ std::string h = m_log.hook("cmp_combo_tree");

    /*debug*/ m_log.htrace(h, "iterate tokens") << "\n";
    using token_iter = std::vector<std::shared_ptr<xtoken>>::const_iterator;
    for (token_iter token = tokens.begin(); token != tokens.end(); ++token) {
        /*debug*/ m_log.htrace(h, "processing token");
        /*debug*/ token->get()->json(0, false, 0, true);

        switch (token->get()->type()) {
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

        /*debug*/ m_log.htrace(h, "created node") << "\n";
        // clang-format off
        /*debug*/ for (auto n : m_nodes)
        /*debug*/     n->json(0, false, 0, false);
        // clang-format on
    }
}

void xsyntaxtree::cat_expression() {
    /*debug*/ auto h = m_log.hook("cat_expr");

    std::shared_ptr<xnode> right_node(m_nodes.back());
    /*debug*/ m_log.htrace(h, "right");
    /*debug*/ right_node->json(0, false, 0, true, right_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> left_node(m_nodes.back());
    /*debug*/ m_log.out << m_log.op("left");
    /*debug*/ left_node->json(0, false, 0, true, left_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> node = regex_node(xnodetype::LEXER_NODE_CAT);
    /*debug*/ m_log.out << m_log.op("cat");
    /*debug*/ node->json(0, false, 0, true, node.use_count());

    node->add_node(left_node);
    node->add_node(right_node);
    m_nodes.push_back(node);
}

void xsyntaxtree::star_expression() {
    /*debug*/ auto h = m_log.hook("star_expr");

    std::shared_ptr<xnode> star_node(m_nodes.back());
    /*debug*/ m_log.htrace(h, "starnode");
    /*debug*/ star_node->json(0, false, 0, true, star_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> node = regex_node(xnodetype::LEXER_NODE_STAR);
    /*debug*/ m_log.htrace(h, "star");
    /*debug*/ node->json(0, false, 0, true, node.use_count());

    node->add_node(star_node);
    m_nodes.push_back(node);
}

void xsyntaxtree::plus_expression() {
    /*debug*/ auto h = m_log.hook("plus_expr");

    std::shared_ptr<xnode> plus_node(m_nodes.back());
    /*debug*/ m_log.htrace(h, "plusnode");
    /*debug*/ plus_node->json(0, false, 0, true, plus_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> node = regex_node(xnodetype::LEXER_NODE_PLUS);
    /*debug*/ m_log.htrace(h, "plus");
    /*debug*/ node->json(0, false, 0, true, node.use_count());

    node->add_node(plus_node);
    m_nodes.push_back(node);
}

void xsyntaxtree::optional_expression() {
    /*debug*/ auto h = m_log.hook("optional_expr");

    std::shared_ptr<xnode> optional_node(m_nodes.back());
    /*debug*/ m_log.htrace(h, "optionalnode");
    /*debug*/ optional_node->json(0, false, 0, true, optional_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> node = regex_node(xnodetype::LEXER_NODE_OPTIONAL);
    /*debug*/ m_log.htrace(h, "optional");
    /*debug*/ node->json(0, false, 0, true, node.use_count());

    node->add_node(optional_node);
    m_nodes.push_back(node);
}

void xsyntaxtree::or_expression() {
    /*debug*/ auto h = m_log.hook("or_expr");

    std::shared_ptr<xnode> right_node(m_nodes.back());
    /*debug*/ m_log.htrace(h, "right");
    /*debug*/ right_node->json(0, false, 0, true, right_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> left_node(m_nodes.back());
    /*debug*/ m_log.out << m_log.op("left");
    /*debug*/ left_node->json(0, false, 0, true, left_node.use_count());
    m_nodes.pop_back();

    std::shared_ptr<xnode> node = regex_node(xnodetype::LEXER_NODE_OR);
    /*debug*/ m_log.out << m_log.op("cat");
    /*debug*/ node->json(0, false, 0, true, node.use_count());

    node->add_node(left_node);
    node->add_node(right_node);
    m_nodes.push_back(node);
}

void xsyntaxtree::begin_bracket_expression() {
    m_bracket_expression_characters.clear();
}

void xsyntaxtree::begin_negative_bracket_expression() {
    m_bracket_expression_characters.clear();
    insert_characters(xnode::BEGIN_CHARACTER, xnode::END_CHARACTER);
}

void xsyntaxtree::end_bracket_expression() {
    std::set<xchars>::const_iterator character = m_bracket_expression_characters.begin();

    std::shared_ptr<xnode> node = regex_node(character->begin_char(), character->end_char());
    m_nodes.push_back(node);
    ++character;

    while (character != m_bracket_expression_characters.end()) {
        node = regex_node(character->begin_char(), character->end_char());
        m_nodes.push_back(node);
        or_expression();
        ++character;
    }
}

void xsyntaxtree::action_expression(const std::string &identifier) {
    assert(!identifier.empty());
    assert(m_generator.get());
    std::shared_ptr<xnode> node = regex_node(m_generator->add_lexer_action(identifier));
    m_nodes.push_back(node);
}

void xsyntaxtree::negative_item_character(int character) {
    assert(character >= xnode::BEGIN_CHARACTER && character < xnode::END_CHARACTER);
    erase_characters(character, character + 1);
}

void xsyntaxtree::item_character(int character) {
    assert(character >= xnode::BEGIN_CHARACTER && character < xnode::END_CHARACTER);
    insert_characters(character, character + 1);
}

void xsyntaxtree::dot() {
    std::shared_ptr<xnode> node = regex_node(xnode::BEGIN_CHARACTER, xnode::END_CHARACTER);
    m_nodes.push_back(node);
}

void xsyntaxtree::character(int character) {
    std::shared_ptr<xnode> node = regex_node(character, character + 1);
    m_nodes.push_back(node);
}

void xsyntaxtree::item_alnum() {
    item_alpha();
    item_digit();
}

void xsyntaxtree::item_word() {
    item_alpha();
    item_digit();
    insert_characters('_', '_' + 1);
}

void xsyntaxtree::item_alpha() {
    item_lower();
    item_upper();
}

void xsyntaxtree::item_blank() {
    insert_characters(" \t");
}

void xsyntaxtree::item_cntrl() {
    insert_characters(0x00, 0x1f + 1);
    insert_characters(0x7f, 0x7f + 1);
}

void xsyntaxtree::item_digit() {
    insert_characters('0', '9' + 1);
}

void xsyntaxtree::item_graph() {
    insert_characters(0x21, 0x7e + 1);
}

void xsyntaxtree::item_lower() {
    insert_characters('a', 'z' + 1);
}

void xsyntaxtree::item_print() {
    insert_characters(0x20, 0x7e + 1);
}

void xsyntaxtree::item_punct() {
    insert_characters("-!\"#$%&'()*+,./:;<=>?@[\\]_`{|}~");
}

void xsyntaxtree::item_space() {
    insert_characters(" \t\r\n");
}

void xsyntaxtree::item_upper() {
    insert_characters('A', 'Z' + 1);
}

void xsyntaxtree::item_xdigit() {
    insert_characters("0123456789abcdefABCDEF");
}

void xsyntaxtree::item_range(int begin, int end) {
    assert(begin >= xnode::BEGIN_CHARACTER && begin < xnode::END_CHARACTER);
    assert(end >= xnode::BEGIN_CHARACTER && end < xnode::END_CHARACTER);
    assert(begin <= end);
    insert_characters(begin, end);
}

void xsyntaxtree::negative_item_alnum() {
    negative_item_alpha();
    negative_item_digit();
}

void xsyntaxtree::negative_item_word() {
    negative_item_alpha();
    negative_item_digit();
    insert_characters('_', '_' + 1);
}

void xsyntaxtree::negative_item_alpha() {
    negative_item_lower();
    negative_item_upper();
}

void xsyntaxtree::negative_item_blank() {
    insert_characters("\t");
}

void xsyntaxtree::negative_item_cntrl() {
    insert_characters(0x00, 0x1f + 1);
    insert_characters(0x7f, 0x7f + 1);
}

void xsyntaxtree::negative_item_digit() {
    insert_characters('0', '9' + 1);
}

void xsyntaxtree::negative_item_graph() {
    insert_characters(0x21, 0x7e + 1);
}

void xsyntaxtree::negative_item_lower() {
    insert_characters('a', 'z' + 1);
}

void xsyntaxtree::negative_item_print() {
    insert_characters(0x20, 0x7e + 1);
}

void xsyntaxtree::negative_item_punct() {
    insert_characters("-!\"#$%&'()*+,./:;<=>?@[\\]_`{|}~");
}

void xsyntaxtree::negative_item_space() {
    insert_characters(" \t\r\n");
}

void xsyntaxtree::negative_item_upper() {
    insert_characters('A', 'Z' + 1);
}

void xsyntaxtree::negative_item_xdigit() {
    insert_characters("0123456789abcdefABCDEF");
}

void xsyntaxtree::negative_item_range(int begin, int end) {
    assert(begin >= xnode::BEGIN_CHARACTER && begin < xnode::END_CHARACTER);
    assert(end >= xnode::BEGIN_CHARACTER && end < xnode::END_CHARACTER);
    assert(begin <= end);
    erase_characters(begin, end);
}

void xsyntaxtree::parse_regular_expression(const std::shared_ptr<xtoken> &token) {
    /*debug*/ std::string h = m_log.hook("parse_regex");
    /*debug*/ m_log.htrace(h, "regex token");
    /*debug*/ token->json(0, false, 0, true);

    assert(token->type() == xtokentype::TOKEN_REGULAR_EXPRESSION);
    assert(!token->lexeme().empty());

    const std::string &regular_expression = token->lexeme();

    xparser parser;
    bool successful = parser.parse(regular_expression.begin(), regular_expression.end(), self());

    if (!successful) {
        ++m_errors;
        /*error*/ m_log.ehtrace(h, "ERROR !parse_regex()") << "type: " << ecode::E_LEXER_SYNTAX << ", ";
        /*error*/ m_log.err << "line: 1, #errors: " << m_errors << ", ";
        /*error*/ m_log.err << "syntax error in regex `" << token->lexeme() << "`!\n";
        m_nodes.clear();
    } else {
        // add the end char to the regex that has just been
        // parsed and the combine that regex with any literals
        // or regex that have benn previously parsed using
        // an or expression
        assert(m_nodes.size() == 1 | m_nodes.size() == 2);
        assert(m_nodes.back().get());

        std::shared_ptr<xnode> node = regex_node(xnode::INVALID_BEGIN_CHARACTER, xnode::INVALID_END_CHARACTER, token);
        m_nodes.push_back(node);
        cat_expression();

        while (m_nodes.size() > 1)
            or_expression();
    }
}

void xsyntaxtree::parse_literal(const std::shared_ptr<xtoken> &token) {
    /*debug*/ std::string h = m_log.hook("parse_literal");
    /*debug*/ m_log.htrace(h, "literal token");
    /*debug*/ token->json(0, false, 0, true);

    assert(token->type() == xtokentype::TOKEN_LITERAL);
    assert(!token->lexeme().empty());
    assert(token->symbol());

    const std::string &literal = token->lexeme();
    std::string::const_iterator i = literal.begin();

    int character = escape(i, literal.end(), &i);

    std::shared_ptr<xnode> node = regex_node(character, character + 1);
    m_nodes.push_back(node);

    ++i;

    while (i != literal.end()) {
        int character = escape(i, literal.end(), &i);

        node = regex_node(character, character + 1);
        m_nodes.push_back(node);
        cat_expression();
        ++i;
    }

    /// add the end char to the literla that has just been parsed and
    /// then combine that literal with any literals or regexs that have
    /// been previously parsed using an or expression

    assert(m_nodes.size() == 1 || m_nodes.size() == 2);
    assert(m_nodes.back().get());

    node = regex_node(xnode::INVALID_BEGIN_CHARACTER, xnode::INVALID_END_CHARACTER, token);
    m_nodes.push_back(node);
    cat_expression();

    while (m_nodes.size() > 1)
        or_expression();
}

std::shared_ptr<xnode> xsyntaxtree::regex_node(int begin, int end) {
    std::shared_ptr<xnode> node = std::make_shared<xnode>(m_index, begin, end);
    ++m_index;
    return node;
}

std::shared_ptr<xnode> xsyntaxtree::regex_node(xnodetype type) {
    std::shared_ptr<xnode> node = std::make_shared<xnode>(m_index, type);
    ++m_index;
    return node;
}

std::shared_ptr<xnode> xsyntaxtree::regex_node(int begin, int end, const std::shared_ptr<xtoken> &token) {
    std::shared_ptr<xnode> node = std::make_shared<xnode>(m_index, begin, end, token);
    ++m_index;
    return node;
}

std::shared_ptr<xnode> xsyntaxtree::regex_node(const std::shared_ptr<xaction> &action) {
    std::shared_ptr<xnode> node = std::make_shared<xnode>(m_index, action);
    ++m_index;
    return node;
}

int xsyntaxtree::escape(std::string::const_iterator start, std::string::const_iterator end, std::string::const_iterator *next) {
    // m_log.out << m_log.op("next") << &next << " " << *next << "\n";
    // m_log.out << m_log.op("next2") << &*next << " " << **next << "\n";
    int character = *start;
    if (character == '\\') {
        ++start;
        if (start != end) {
            switch (*start) {
            case 'b':
                character = '\b';
                break;
            case 'f':
                character = '\f';
                break;
            case 'n':
                character = '\n';
                break;
            case 'r':
                character = '\r';
                break;
            case 't':
                character = '\t';
                break;
            case 'x':
            case 'X':
                assert(false);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                assert(false);
                break;
            default:
                character = *start;
                break;
            }
        }
    }
    // assert(&next);
    // next = start;
    // m_log.out << m_log.op("next") << &next << " " << *next << "\n\n";
    assert(next);
    *next = start;
    // m_log.out << m_log.op("next2") << &*next << " " << **next << "\n\n";
    return character;
}

void xsyntaxtree::insert_characters(int begin, int end) {
    std::pair<std::set<xchars>::iterator, bool> result = m_bracket_expression_characters.insert(xchars(begin, end));

    while (!result.second) {
        assert(result.first->end_char() >= begin || end >= result.first->begin_char());

        begin = std::min(begin, result.first->begin_char());
        end = std::max(end, result.first->end_char());

        m_bracket_expression_characters.erase(result.first);

        result = m_bracket_expression_characters.insert(xchars(begin, end)); // useless assignment
    }
}

// TODO FIXME test this method
void xsyntaxtree::insert_characters(const std::string &characters) {
    assert(!characters.empty());
    using char_iter = std::string::const_iterator;
    for (char_iter character = characters.begin(); character != characters.end(); ++character) {
        insert_characters(*character, *character + 1);
    }
    m_log.out << m_log.cred << "CHECK THIS FUNCTION!\n";
    assert(false);
}

void xsyntaxtree::erase_characters(int begin, int end) {
    std::set<xchars>::iterator i = m_bracket_expression_characters.find(xchars(begin, end));

    if (i != m_bracket_expression_characters.end()) {
        int pre_begin = i->begin_char();
        int pre_end = begin;

        int post_begin = end;
        int post_end = i->end_char();

        m_bracket_expression_characters.erase(i);

        if (pre_begin < pre_end) {
            bool inserted = m_bracket_expression_characters.insert(xchars(pre_begin, pre_end)).second;
            assert(inserted);
            (void)inserted; // TODO why? useless?
        }

        if (post_begin < post_end) {
            bool inserted = m_bracket_expression_characters.insert(xchars(post_begin, post_end)).second;
            assert(inserted);
            (void)inserted; // TODO why? useless?
        }
    }
}
