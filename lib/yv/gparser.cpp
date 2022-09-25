#include "include/gparser.hpp"
#include "include/ecode.hpp"
#include "include/ggrammar.hpp"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

gparser::gparser()
    : m_grammar(),
      m_position(),
      m_end(),
      m_line(1),
      m_errors(0),
      m_lexeme(),
      m_log("yyv", "gpars", 214) {
    m_lexeme.reserve(256);
}

int gparser::parse(std::string::iterator &start, std::string::iterator &finish, const std::shared_ptr<ggrammar> &grammar) {
    /*debug*/ std::string h = m_log.hook("parse");

    assert(&start);
    assert(&finish);
    assert(grammar.get());

    m_position = start;
    m_end = finish;
    /*debug*/ m_log.htrace(h, "interval (params)") << "[posit, fin) ";
    /*debug*/ m_log.out << m_log.chl << "[" << (void *)&*m_position;
    /*debug*/ m_log.out << ", " << (void *)&*m_end << ")\n";

    m_grammar = grammar;
    /*debug*/ m_log.out << m_log.op("grammar (shared copied)") << "address ";
    /*debug*/ m_log.out << m_log.chl << &*m_grammar << m_log.cnr << " use_count ";
    /*debug*/ m_log.out << m_log.chl << m_grammar.use_count() << "\n";

    m_line = 1;
    m_errors = 0;

    /*debug*/ m_log.out << m_log.op("initialize") << "m_line " << m_log.chl << m_line << "\n";
    /*debug*/ m_log.out << m_log.op("initialize") << "m_errors " << m_log.chl << m_errors << "\n";

    if (!match_grammar()) {
        ++m_errors;
        /*error*/ m_log.ehtrace(h, "ERROR !match_grammar()") << "type: " << ecode::E_SYNTAX << ", ";
        /*error*/ m_log.err << "line: 1, #errors: " << m_errors << ", ";
        /*error*/ m_log.err << "parsing grammar failed!\n";
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
        m_grammar->literal(m_lexeme, m_line);
        return true;
    } else if (match_regex()) {
        m_grammar->regex(m_lexeme, m_line);
        return true;
    } else if (match_identifier()) {
        m_grammar->identifier(m_lexeme, m_line);
        return true;
    }
    return false;
}

bool gparser::match_error() {
    return match("error");
}

bool gparser::match_literal() {
    /*debug*/ std::string h = m_log.hook("match_literal");

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
            /*debug*/ m_log.htrace(h, "matched LITERAL") << m_log.chl << m_lexeme << "\n";

            m_position = position;
            expect("'");
            return true;
        }
        ++m_errors;
        /*error*/ m_log.ehtrace(h, "ERROR !match_literal()") << "type: " << ecode::E_SYNTAX << ", ";
        /*error*/ m_log.err << "line: " << m_line << ", #errors: " << m_errors << ", ";
        /*error*/ m_log.err << "unterminated literal!";

        return false;
    }
    return false;
}

bool gparser::match_regex() {
    /*debug*/ std::string h = m_log.hook("match_regex");

    match_whitespace_and_comments();
    if (match("\"")) {
        bool escaped = false;
        std::string::iterator position = m_position;
        while (position != m_end && (*position != '"' || escaped)) {
            escaped = *position == '\\';
            ++position;
        }
        m_lexeme.assign(m_position, position);
        /*debug*/ m_log.htrace(h, "matched REGEX") << m_log.chl << m_lexeme << "\n";

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
            m_grammar->regex(m_lexeme, m_line);
        }
        expect(";");
        return true;
    }
    return false;
}

bool gparser::match_production_statement() {
    if (match_identifier()) {
        m_grammar->production(m_lexeme, m_line);
        expect(":");
        match_expressions();
        expect(";");
        m_grammar->end_production();
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
            m_grammar->action(m_lexeme, m_line);
        }
        expect("]");
        return true;
    }
    m_grammar->end_expression(m_line);
    return false;
}

bool gparser::match_identifier() {
    /*debug*/ std::string h = m_log.hook("match_identifier");

    match_whitespace_and_comments();
    // TODO maybe extract to is_identifier_char (maybe init char w/ parameter)
    std::string::iterator position = m_position;
    if (position != m_end && (std::isalnum(*position) || *position == '_')) {
        ++position;
        while (position != m_end && (std::isalnum(*position) || std::isdigit(*position) || *position == '_'))
            ++position;
        m_lexeme.assign(m_position, position);
        /*debug*/ m_log.htrace(h, "matched IDENTIFIER") << m_log.chl << m_lexeme << "\n";

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
    // TODO maybe extract to is_whitespace_char
    std::string::iterator position = m_position;
    if (position != m_end && std::isspace(*position)) {
        while (position != m_end && std::isspace(*position)) {
            if (is_new_line(position))
                ++m_line;
            ++position;
        }
        m_position = position;
        return true;
    }
    return false;
}

bool gparser::match_line_comment() {
    /*debug*/ std::string h = m_log.hook("match_line_comment");

    if (match_without_skipping_whitespace("//")) {
        std::string::iterator position = m_position;
        while (position != m_end && !is_new_line(position))
            ++position;
        /*debug*/ auto s = std::string(m_position, position);
        /*debug*/ auto l = (s.size() < 20 ? s.size() : 20);
        /*debug*/ m_log.htrace(h, "ignored LINE COMMENT") << m_log.chl << s.substr(0, l) << " ...\n";

        m_position = new_line(position);
        return true;
    }
    return false;
}

bool gparser::match_block_comment() {
    /*debug*/ std::string h = m_log.hook("match_block_comment");

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
        /*debug*/ auto s = std::string(m_position, position);
        /*debug*/ auto l = (s.size() < 20 ? s.size() : 20);
        /*debug*/ m_log.htrace(h, "ignored BLOCK COMMENT") << m_log.chl << s.substr(0, l) << " ...\n";

        m_position = position;
        return true;
    }
    return false;
}

bool gparser::match_without_skipping_whitespace(const std::string &plexeme) {
    /*debug*/ std::string h = m_log.hook("match_without_skipping_whitespace");

    std::string::const_iterator lexeme = plexeme.begin();
    std::string::iterator position = m_position;
    while (position != m_end && *lexeme != 0 && *position == *lexeme) {
        ++position;
        ++lexeme;
    }
    if (*lexeme == 0) {
        /*debug*/ m_log.htrace(h, "matched NO SKIP WHITESPACE") << m_log.chl << std::string(m_position, position) << "\n";

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
    /*debug*/ std::string h = m_log.hook("expect");

    if (match(lexeme))
        return true;
    m_position = m_end;

    ++m_errors;
    /*error*/ m_log.ehtrace(h, "ERROR !expect()") << "type: " << ecode::E_SYNTAX << ", ";
    /*error*/ m_log.err << "line: " << m_line << ", #errors: " << m_errors << ", ";
    /*error*/ m_log.err << "expected `" << m_log.cwhite << lexeme << m_log.cred << "` not found\n";

    return false;
}

std::string::iterator &gparser::new_line(std::string::iterator &position) {
    if (position != m_end) {
        if (*position == '\n') {
            ++position;
            if (position != m_end && *position == '\r')
                ++position;
            ++m_line;
        } else if (*position == '\r') {
            ++position;
            if (position != m_end && *position == '\n')
                ++position;
            ++m_line;
        }
    }
    return position;
}

bool gparser::is_new_line(std::string::iterator &position) {
    return *position == '\n' || *position == '\r';
}
