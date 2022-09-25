#pragma once

#include "glogger.hpp"
#include "xtokentype.hpp"

#include <memory>
#include <vector>

/// literal/regex to be recognized by a lexical analyzer
class xtoken {
  private:
    /// token type (literal or regex)
    xtokentype m_type;
    /// line to use when resolving token conflicts or reporting errors
    int m_line;
    /// column to use when resolving token conflicts or reporting errors
    int m_column;
    /// symbol to return then this token is matched in input
    const void *m_symbol;
    /// literal/regex pattern to match for this token
    std::string m_lexeme;
    /// the tokens that this token has conflicted with
    mutable std::vector<std::shared_ptr<xtoken>> m_conflicted_with;
    /// logger
    glogger m_log;

  public:
    xtoken(xtokentype type, int line, int column, const void *symbol, const std::string &lexeme);

  public:
    xtokentype type() const;
    int line() const;
    int column() const;
    const void *symbol() const;
    const std::string &lexeme() const;

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
