#pragma once

#include "glogger.hpp"
#include "xchars.hpp"
#include "xnode.hpp"
#include "xnodetype.hpp"
#include "xtoken.hpp"

#include <memory>
#include <set>
#include <vector>

class xgenerator;

class xsyntaxtree : public std::enable_shared_from_this<xsyntaxtree> {
  private:
    /// generator
    std::shared_ptr<xgenerator> m_generator;
    /// the current node index
    int m_index;
    /// the number of errors that have occurred
    int m_errors;
    /// the current nodes
    std::vector<std::shared_ptr<xnode>> m_nodes;
    /// the chars in the current bracket expression
    std::set<xchars> m_bracket_expression_characters;
    /// logger
    glogger m_log;

  public:
    xsyntaxtree();

  public:
    std::shared_ptr<xsyntaxtree> self();

  public:
    void reset(const std::vector<std::shared_ptr<xtoken>> &tokens, const std::shared_ptr<xgenerator> &generator);
    void calculate_combined_parse_tree(const std::vector<std::shared_ptr<xtoken>> &tokens);

    void cat_expression();
    void or_expression();
    void star_expression();
    void plus_expression();
    void optional_expression();

    void begin_bracket_expression();
    void begin_negative_bracket_expression();
    void end_bracket_expression();

    void negative_item_character(int character);

    void item_alnum();
    void item_alpha();
    void item_lower();
    void item_upper();
    void item_digit();

    void negative_item_alnum();
    void negative_item_alpha();
    void negative_item_lower();
    void negative_item_upper();
    void negative_item_digit();

    void parse_regular_expression(const std::shared_ptr<xtoken> &token);
    void parse_literal(const std::shared_ptr<xtoken> &token);

    std::shared_ptr<xnode> regex_node(int begin, int end);
    std::shared_ptr<xnode> regex_node(xnodetype type);
    std::shared_ptr<xnode> regex_node(int begin, int end, const std::shared_ptr<xtoken> &token);

    void insert_characters(int begin, int end);
    void erase_characters(int begin, int end);

    int escape(std::string::const_iterator start, std::string::const_iterator end, std::string::const_iterator *next);
};
