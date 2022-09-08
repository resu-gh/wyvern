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
      m_lexeme(),
      m_log() {
    m_lexeme.reserve(256);
}

int gparser::parse(std::string::iterator &start,
                   std::string::iterator &finish,
                   const std::shared_ptr<ggrammar> &grammar) {
    assert(&start);
    assert(&finish);
    assert(grammar.get());
    // debug begin
    m_log.out << m_log.cgpars << "yv::gpars = ";
    m_log.out << "[b, e) = ";
    m_log.out << "[" << (void *)&*start;
    m_log.out << ", " << (void *)&*finish << ")";
    m_log.out << m_log.creset << "\n";
    // debug end
    m_position = start;
    m_end = finish;
    // debug begin
    m_log.out << m_log.cgpars << "yv::gpars = ";
    m_log.out << "[p, f) = ";
    m_log.out << "[" << (void *)&*m_position;
    m_log.out << ", " << (void *)&*m_end << ")";
    m_log.out << m_log.creset << "\n";
    // debug end
    m_grammar = grammar;
    // debug begin
    m_log.out << m_log.cgpars << "yv::gpars = ";
    m_log.out << "&ggrammar = ";
    m_log.out << (void *)&*m_grammar << " ";
    m_log.out << "[uses: " << m_grammar.use_count() << "]";
    m_log.out << m_log.creset << "\n";
    // debug end
    m_line = 1;
    m_errors = 0;
    if (!match_grammar()) {
        // error begin
        ++m_errors;
        m_log.err << m_log.cerror << "yv::gpars = ";
        m_log.err << "ERROR: " << ecode::E_SYNTAX << " ";
        m_log.err << "on line 1 :: ";
        m_log.err << "parsing grammar failed";
        m_log.err << m_log.creset << "\n";
        // error end
    }
    return m_errors;
}

bool gparser::match_grammar() {
    if (match_identifier()) {
        m_grammar->grammar(m_lexeme);
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
    return match_associativity_statement() || match_whitespace_statement() || match_production_statement();
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
        m_grammar->left(m_line);
        return true;
    } else if (match("%right")) {
        m_grammar->right(m_line);
        return true;
    } else if (match("%none")) {
        m_grammar->none(m_line);
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
        m_grammar->error(m_line);
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
            // debug begin
            m_log.out << m_log.cgpars << "yv::gpars = ";
            m_log.out << "matched LITERAL ";
            m_log.out << m_log.cwhite << m_lexeme;
            m_log.out << m_log.creset << "\n";
            // debug end
            m_position = position;
            expect("'");
            return true;
        }
        // error begin
        ++m_errors;
        m_log.err << m_log.cblue << "[gparser] " << m_log.creset;
        m_log.err << m_log.cred << ecode::E_SYNTAX << " ";
        m_log.err << m_log.cmagenta << "[" << m_line << "] " << m_log.creset;
        m_log.err << "unterminated literal\n";
        // error end
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
        // debug begin
        m_log.out << m_log.cgpars << "yv::gpars = ";
        m_log.out << "matched REGEX ";
        m_log.out << m_log.cwhite << m_lexeme;
        m_log.out << m_log.creset << "\n";
        // debug end
        m_position = position;
        expect("\"");
        return true;
    }
    return false;
}

bool gparser::match_whitespace_statement() {
    if (match("%whitespace")) {
        m_grammar->whitespace(m_line);
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
        m_grammar->production(m_lexeme.c_str(), m_line);
        expect(":");
        match_expressions();
        expect(";");
        m_grammar->end_production(); // done
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
        m_grammar->precedence(m_line);
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
        // debug begin
        m_log.out << m_log.cgpars << "yv::gpars = ";
        m_log.out << "matched IDENTIFIER ";
        m_log.out << m_log.cwhite << m_lexeme;
        m_log.out << m_log.creset << "\n";
        // debug end
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
        // debug begin
        m_log.out << m_log.cgpars << "yv::gpars = ";
        m_log.out << "skip";
        m_log.out << m_log.creset << "\n";
        // debug end
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
        // debug begin
        m_log.out << m_log.cgpars << "yv::gpars = ";
        m_log.out << "matched LINE COMMENT\n";
        m_log.out << m_log.cwhite << std::string(m_position, position);
        m_log.out << m_log.creset << "\n";
        // debug end
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
        // debug begin
        m_log.out << m_log.cgpars << "yv::gpars = ";
        m_log.out << "matched BLOCK COMMENT\n";
        m_log.out << m_log.cwhite << std::string(m_position, position);
        m_log.out << m_log.creset << "\n";
        // debug end
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
        // debug begin
        m_log.out << m_log.cgpars << "yv::gpars = ";
        m_log.out << "noskip ";
        m_log.out << m_log.cwhite << std::string(m_position, position);
        m_log.out << m_log.creset << "\n";
        // debug end
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
    // error begin
    ++m_errors;
    m_log.err << m_log.cerror << "yv::gpars = ";
    m_log.err << "ERROR: " << ecode::E_SYNTAX << " ";
    m_log.err << "on line " << m_line << " :: ";
    m_log.err << "expected `" << lexeme << "` not found";
    m_log.err << m_log.creset << "\n";
    // error end
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
