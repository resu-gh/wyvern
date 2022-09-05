#pragma once

#include "./glogger.hpp"

#include <memory>
#include <string>

class ggrammar;

class gparser {
  private:
    std::string::iterator m_position;
    std::string::iterator m_end;
    std::shared_ptr<ggrammar> m_grammar;
    int m_line;   // uint32_t
    int m_errors; // uint32_t
    std::string m_lexeme;
    glogger m_log;

  public:
    gparser();

  public:
    int parse(std::string::iterator &, std::string::iterator &, const std::shared_ptr<ggrammar> &);

  private:
    bool match_grammar();
    bool match_identifier();                                     // match_ident
    bool match_whitespace_and_comments();                        // match_combo_ignore
    bool match_whitespace();                                     // match_blank
    bool match_line_comment();                                   // match_cline
    bool match_block_comment();                                  // match_cblock
    bool match_without_skipping_whitespace(const std::string &); // match_noskip_blank
    bool match_statements();                                     // match_combo_stmt
    bool match_statement();                                      // match_stmt
    bool match_associativity_statement();                        // match_combo_assoc
    bool match_associativity();                                  // match_assoc
    bool match_symbols();                                        // match_syms
    bool match_symbol();                                         // match_sym
    bool match_error();                                          //
    bool match_literal();                                        //
    bool match_regex();                                          //
    bool match_whitespace_statement();                           // match_blank_stmt
    bool match_production_statement();                           // match_prod_stmt
    bool match_expressions();                                    // match_exprs
    bool match_expression();                                     // match_expr
    bool match_precedence();                                     // match_prec
    bool match_action();                                         // match_action
    bool match_end();                                            // match_post

    bool match(const std::string &);
    bool expect(const std::string &);
    bool is_new_line(std::string::iterator &);                // is_nl
    std::string::iterator &new_line(std::string::iterator &); // nl
};
