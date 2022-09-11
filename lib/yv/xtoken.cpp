#include "include/xtoken.hpp"

xtoken::xtoken(xtokentype type, int line, int column, const std::shared_ptr<void> symbol, const std::string &lexeme)
    : m_type(type),
      m_line(line),
      m_column(column),
      m_symbol(symbol),
      m_lexeme(lexeme),
      m_conflicted_with(),
      m_log("yyv", "xtokn", 255) {}

xtokentype xtoken::type() const {
    return m_type;
}

int xtoken::line() const {
    return m_line;
}

int xtoken::column() const {
    return m_column;
}

const std::shared_ptr<void> &xtoken::symbol() const {
    return m_symbol;
}

const std::string &xtoken::lexeme() const {
    return m_lexeme;
}
