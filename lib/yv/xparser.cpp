#include "include/xparser.hpp"
#include "include/xsyntaxtree.hpp"

#include <cassert>
#include <cctype>
#include <cstring>
#include <memory>

xparser::xparser()
    : m_syntax_tree(),
      m_position(),
      m_end(),
      m_lexeme_begin(),
      m_lexeme_end(),
      m_log("yyv", "xpars", 255),
      m_successful(false) {}

bool xparser::parse(std::string::const_iterator begin, std::string::const_iterator end, const std::shared_ptr<xsyntaxtree> &syntax_tree) {
    m_syntax_tree = syntax_tree;
    m_position = begin;
    m_end = end;
    m_successful = true;
    return match_or_expression() && m_successful;
}

bool xparser::match_or_expression() {
    if (match_cat_expression()) {
        if (match("|") && match_or_expression())
            m_syntax_tree->or_expression();
        return true;
    }
    return false;
}

bool xparser::match_cat_expression() {
    if (match_postfix_expression()) {
        if (match_cat_expression())
            m_syntax_tree->cat_expression();
        return true;
    }
    return false;
}

bool xparser::match_postfix_expression() {
    if (match_base_expression()) {
        if (match("*"))
            m_syntax_tree->star_expression();
        else if (match("+"))
            m_syntax_tree->plus_expression();
        else if (match("?"))
            m_syntax_tree->optional_expression();
        return true;
    }
    return false;
}

bool xparser::match_base_expression() {
    return match_negative_bracket_expression();
}

bool xparser::match_negative_bracket_expression() {
    if (match("[^")) {
        m_syntax_tree->begin_negative_bracket_expression();
        if (match("-"))
            m_syntax_tree->negative_item_character('-');
        while (match_negative_item())
            ;
        if (match("-"))
            m_syntax_tree->negative_item_character('-');
        expect("]");
        m_syntax_tree->end_bracket_expression();
        return true;
    }
    return false;
}

bool xparser::match_negative_item() {
    if (match("[:alnum:]")) {
        m_syntax_tree->negative_item_alnum();
        return true;
    } else if (match("[:word:]")) {
        m_syntax_tree->negative_item_word();
        return true;
    } else if (match("[:alpha:]")) {
        m_syntax_tree->negative_item_alpha();
        return true;
    } else if (match("[:blank:]")) {
        m_syntax_tree->negative_item_blank();
        return true;
    } else if (match("[:cntrl:]")) {
        m_syntax_tree->negative_item_cntrl();
        return true;
    } else if (match("[:digit:]")) {
        m_syntax_tree->negative_item_digit();
        return true;
    } else if (match("[:graph:]")) {
        m_syntax_tree->negative_item_graph();
        return true;
    } else if (match("[:lower:]")) {
        m_syntax_tree->negative_item_lower();
        return true;
    } else if (match("[:print:]")) {
        m_syntax_tree->negative_item_print();
        return true;
    } else if (match("[:punct:]")) {
        m_syntax_tree->negative_item_punct();
        return true;
    } else if (match("[:space:]")) {
        m_syntax_tree->negative_item_space();
        return true;
    } else if (match("[:upper:]")) {
        m_syntax_tree->negative_item_upper();
        return true;
    } else if (match("[:xdigit:]")) {
        m_syntax_tree->negative_item_xdigit();
        return true;
    } else if (match_character()) {
        int character = escape(m_lexeme_begin, m_lexeme_end);
        if (match_end_of_range()) {
            int end_character = escape(m_lexeme_begin, m_lexeme_end) + 1;
            m_syntax_tree->negative_item_range(character, end_character);
        } else {
            m_syntax_tree->negative_item_character(character);
        }
        return true;
    }
    return false;
}

// TODO FIXME test
bool xparser::match_character() {
    std::string::const_iterator position = m_position;
    if (position == m_end) {
        if (*position == '\\') {
            ++position;
            if (position != m_end && (*position == 'x' || *position == 'X')) {
                ++position;
                while (position != m_end && std::isxdigit(*position))
                    ++position;
            } else if (position != m_end && std::isdigit(*position)) {
                ++position;
                while (position != m_end && std::isdigit(*position))
                    ++position;
            } else if (position != m_end) {
                ++position;
            }
            m_lexeme_begin = m_position;
            m_lexeme_end = position;
            m_position = position;
            return true;
        } else if (!std::strchr("|*+?[]()-", *position)) {
            ++position;
            m_lexeme_begin = m_position;
            m_lexeme_end = position;
            m_position = position;
            return true;
        }
    }
    return false;
}

bool xparser::match_end_of_range() {
    std::string::const_iterator position = m_position;
    if (match("-") && match_character())
        return true;
    m_position = position;
    return false;
}

// TODO FIXME custom impl
bool xparser::match(const std::string &lexeme) {
    std::string::const_iterator lexpos = lexeme.begin();
    std::string::const_iterator position = m_position;

    while (position != m_end && lexpos != lexeme.end() && *position == *lexpos) {
        ++position;
        ++lexpos;
    }

    if (lexpos == lexeme.end()) {
        m_lexeme_begin = m_position;
        m_lexeme_end = position;
        m_position = position;
        return true;
    }

    return false;
}

// TODO FIXME custom impl
bool xparser::expect(const std::string &lexeme) {
    if (match(lexeme))
        return true;
    m_position = m_end;
    m_successful = false;
    return false;
}

// TODO FIXME test
int xparser::escape(std::string::const_iterator start, std::string::const_iterator end) const {
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
                ++start;
                character = 0;
                while (std::isxdigit(*start)) {
                    character <<= 4;
                    if (*start >= '0' && *start <= '9')
                        character += *start - '0';
                    else if (*start >= 'a' && *start <= 'f')
                        character += *start - 'a' + 10;
                    else if (*start >= 'A' && *start <= 'F')
                        character += *start - 'A' + 10;
                    ++start;
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7': {
                character = 0;
                int count = 0;
                while (*start >= '0' && *start <= '7' && count < 3) {
                    character <<= 3;
                    character += *start - '0';
                    ++start;
                    ++count;
                }
                break;
            }
            default:
                character = *start;
                break;
            }
        }
    }
    return character;
}
