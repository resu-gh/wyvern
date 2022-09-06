#include "include/gparser.hpp"
#include "include/ecode.hpp"
#include "include/ggrammar.hpp"

#include <cassert>
#include <cctype>
#include <iostream>
#include <string>

gparser::gparser()
    : m_position(nullptr),
      m_end(nullptr),
      m_grammar(nullptr),
      m_line(1),
      m_errors(0),
      m_lexeme() {
    m_lexeme.reserve(256);
}

int gparser::parse(std::string::iterator &start,
                   std::string::iterator &finish,
                   const std::shared_ptr<ggrammar> &grammar) {
    assert(&start);
    assert(&finish);
    assert(grammar.get());
    printf("gpar [b, e)  = [%p, %p)\n", (void *)&*start, (void *)&*finish);
    m_position = start;
    m_end = finish;
    printf("gpar [p, f)  = [%p, %p)\n", (void *)&*m_position, (void *)&*m_end);
    m_grammar = grammar;
    printf("gpar grammar = %p %ld\n", (void *)&*m_grammar, m_grammar.use_count());
    m_line = 1;
    m_errors = 0;
    return 0; // TODO remove
    if (!match_grammar()) {
        ++m_errors;
        m_log.err << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.err << m_log.cred << ecode::E_SYNTAX << " ";
        m_log.err << m_log.cmagenta << "[1] " << m_log.creset;
        m_log.err << "parsing grammar failed\n";
    }
    return m_errors;
}

bool gparser::match_grammar() {
    if (match_identifier()) {
        // m_grammar->grammar(m_lexeme.c_str());
        expect("{");
        match_statements();
        expect("}");
        return match_end();
    }
    return false;
}

bool gparser::match_statements() {
    while (match_statement())
        ;
    return true;
}

bool gparser::match_statement() {
    return                                 //
        match_associativity_statement() || //
        match_whitespace_statement() ||    //
        match_production_statement();      //
}

bool gparser::match_associativity_statement() {
    if (match_associativity()) {
        match_symbols();
        expect(";");
        return true;
    }
    return false;
}

bool gparser::match_associativity() {
    if (match("%left")) {
        // m_grammar->left(m_line);
        return true;
    } else if (match("%right")) {
        // m_grammar->right(m_line);
        return true;
    } else if (match("%none")) {
        // m_grammar->none(m_line);
        return true;
    }
    return false;
}

bool gparser::match_symbols() {
    while (match_symbol())
        ;
    return true;
}

bool gparser::match_symbol() {
    if (match_error()) {
        // m_grammar->error(m_line);
        return true;
    } else if (match_literal()) {
        // m_grammar->literal(m_lexeme.c_str(), m_line);
        return true;
    } else if (match_regex()) {
        // m_grammar->regex(m_lexeme.c_str(), m_line)
        return true;
    } else if (match_identifier()) {
        // m_grammar->identifier(m_lexeme.c_str(), m_line);
        return true;
    }
    return false;
}

bool gparser::match_error() {
    return match("error");
}

bool gparser::match_literal() {
    match_whitespace_and_comments();
    if (match("'")) {
        bool escaped = false;
        std::string::iterator position = m_position;
        while (position != m_end && (*position != '\'' || escaped) && !is_new_line(position)) {
            escaped = *position == '\\';
            ++position;
        }
        if (position == m_end || !is_new_line(position)) {
            m_lexeme.assign(m_position, position);
            m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
            m_log.out << m_log.cgreen << "MATCH " << m_log.creset << "literal ";
            m_log.out << m_log.cyellow << m_lexeme << "\n";
            m_log.out << m_log.creset;
            m_position = position;
            expect("'");
            return true;
        }
        ++m_errors;
        m_log.err << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.err << m_log.cred << ecode::E_SYNTAX << " ";
        m_log.err << m_log.cmagenta << "[" << m_line << "] " << m_log.creset;
        m_log.err << "unterminated literal\n";
        return false;
    }
    return false;
}

bool gparser::match_regex() {
    match_whitespace_and_comments();
    if (match("\"")) {
        bool escaped = false;
        std::string::iterator position = m_position;
        while (position != m_end && (*position != '"' || escaped)) {
            escaped = *position == '\\';
            ++position;
        }
        m_lexeme.assign(m_position, position);
        m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.out << m_log.cgreen << "MATCH " << m_log.creset << "regex ";
        m_log.out << m_log.cyellow << m_lexeme << "\n";
        m_log.out << m_log.creset;
        m_position = position;
        expect("\"");
        return true;
    }
    return false;
}

bool gparser::match_whitespace_statement() {
    if (match("%whitespace")) {
        // m_grammar->whitespace();
        if (match_regex()) {
            // m_grammar->regex(m_lexeme.c_str(),m_line);
        }
        expect(";");
        return true;
    }
    return false;
}

bool gparser::match_production_statement() {
    if (match_identifier()) {
        // m_grammar->production(m_lexeme.c_str(), m_line);
        expect(":");
        match_expressions();
        expect(";");
        // m_grammar->end_production();
        return true;
    }
    return false;
}

bool gparser::match_expressions() {
    match_expression();
    while (match("|") && match_expression())
        ;
    return true;
}

bool gparser::match_expression() {
    match_symbols();
    match_precedence();
    match_action();
    return true;
}

bool gparser::match_precedence() {
    if (match("%precedence")) {
        // m_grammar->precedence();
        match_symbol();
        return true;
    }
    return false;
}

bool gparser::match_action() {
    if (match("[")) {
        if (match_identifier()) {
            // m_grammar->action(m_lexeme.c_str(), m_line);
        }
        expect("]");
        return true;
    }
    // m_grammar->end_expression(m_line);
    return false;
}

bool gparser::match_identifier() {
    match_whitespace_and_comments();
    // extract to is_identifier_char (maybe init char w/ parameter)
    std::string::iterator position = m_position;
    if (position != m_end && (std::isalnum(*position) || *position == '_')) {
        ++position;
        while (position != m_end && (std::isalnum(*position) || std::isdigit(*position) || *position == '_'))
            ++position;
        m_lexeme.assign(m_position, position);
        m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.out << m_log.cgreen << "MATCH " << m_log.creset << "identifier ";
        m_log.out << m_log.cyellow << m_lexeme << "\n";
        m_log.out << m_log.creset;
        m_position = position;
        return true;
    }
    return false;
}

bool gparser::match_whitespace_and_comments() {
    while (match_whitespace() || match_line_comment() || match_block_comment())
        ;
    return true;
}

bool gparser::match_whitespace() {
    // extract to is_whitespace_char
    std::string::iterator position = m_position;
    if (position != m_end && std::isspace(*position)) {
        while (position != m_end && std::isspace(*position)) {
            if (is_new_line(position))
                ++m_line;
            ++position;
        }
        m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.out << m_log.ccyan << "SKIP\n";
        // m_log.out << " (" << m_log.cyellow << std::string(m_position, position) << m_log.cgreen << ")\n";
        m_log.out << m_log.creset;
        m_position = position;
        return true;
    }
    return false;
}

bool gparser::match_line_comment() {
    if (match_without_skipping_whitespace("//")) {
        std::string::iterator position = m_position;
        while (position != m_end && !is_new_line(position))
            ++position;
        m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.out << m_log.cgreen << "MATCH " << m_log.creset << "line comment\n";
        m_log.out << m_log.cyellow << std::string(m_position, position) << "\n";
        m_log.out << m_log.creset;
        m_position = new_line(position);
        return true;
    }
    return false;
}

bool gparser::match_block_comment() {
    if (match_without_skipping_whitespace("/*")) {
        bool done = false;
        std::string::iterator position = m_position;
        while (position != m_end && !done) {
            if (*position == '*') {
                ++position;
                if (position != m_end && *position == '/') {
                    done = true;
                    ++position;
                }
            } else if (is_new_line(position)) {
                position = new_line(position);
            } else {
                ++position;
            }
        }
        m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.out << m_log.cgreen << "MATCH " << m_log.creset << "block comment\n";
        m_log.out << m_log.cyellow << std::string(m_position, position) << "\n";
        m_log.out << m_log.creset;
        m_position = position;
        return true;
    }
    return false;
}

bool gparser::match_without_skipping_whitespace(const std::string &plexeme) {
    std::string::const_iterator lexeme = plexeme.begin();
    std::string::iterator position = m_position;
    while (position != m_end && *lexeme != 0 && *position == *lexeme) {
        ++position;
        ++lexeme;
    }
    if (*lexeme == 0) {
        m_log.out << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.out << m_log.ccyan << "NOSKIP ";
        m_log.out << m_log.cyellow << std::string(m_position, position) << "\n";
        m_log.out << m_log.creset;
        m_position = position;
        return true;
    }
    return false;
}

bool gparser::match_end() {
    match_whitespace_and_comments();
    return m_position == m_end;
}

bool gparser::match(const std::string &keyword) {
    match_whitespace_and_comments();
    return match_without_skipping_whitespace(keyword);
}

bool gparser::expect(const std::string &lexeme) {
    if (match(lexeme))
        return true;
    m_position = m_end;
    ++m_errors;
    m_log.err << m_log.cblue << "[gparser] " << m_log.creset;
    m_log.err << m_log.cred << ecode::E_SYNTAX << " ";
    m_log.err << m_log.cmagenta << "[" << m_line << "] " << m_log.creset;
    m_log.err << "expected `" << m_log.cyellow << lexeme;
    m_log.err << m_log.creset << "` not found\n";
    return false;
}

std::string::iterator &gparser::new_line(std::string::iterator &position) {
    if (position != m_end) {
        if (*position == '\n') {
            ++position;
            if (position != m_end && *position == '\r') {
                ++position;
            }
            ++m_line;
        } else if (*position == '\r') {
            ++position;
            if (position != m_end && *position == '\n') {
                ++position;
            }
            ++m_line;
        }
    }
    // if (position != m_end && *position == '\n') {
    //     ++position;
    //     if (position != m_end && *position == '\r')
    //         ++position;
    //     ++m_line;
    // } else if (position != m_end && *position == '\r') {
    //     ++position;
    //     if (position != m_end && *position == '\n')
    //         ++position;
    //     ++m_line;
    // }
    return position;
}

bool gparser::is_new_line(std::string::iterator &position) {
    return *position == '\n' || *position == '\r';
}