#pragma once

#include "glogger.hpp"
#include "xsyntaxtree.hpp"

#include <memory>
#include <string>

class xparser {
  private:
    /// syntax tree
    std::shared_ptr<xsyntaxtree> m_syntax_tree;
    ///
    std::string::const_iterator m_position;
    std::string::const_iterator m_end;
    std::string::const_iterator m_lexeme_begin;
    std::string::const_iterator m_lexeme_end;
    bool m_successful;
    /// logger
    glogger m_log;

  public:
    xparser();

  public:
    bool parse(std::string::const_iterator begin, std::string::const_iterator end, const std::shared_ptr<xsyntaxtree> &syntax_tree);

  private:
    bool match_or_expression();
    bool match_cat_expression();
    bool match_postfix_expression();
    bool match_base_expression();
    bool match_negative_bracket_expression();
    bool match_bracket_expression();
    bool match_action_expression();
    bool match_negative_item();
    bool match_item();
    bool match_identifier();
    bool match_character();
    bool match_end_of_range();

    bool match(const std::string &lexeme);
    bool expect(const std::string &lexeme);

    int escape(std::string::const_iterator start, std::string::const_iterator end) const;
};
