#pragma once

#include "gaction.hpp"
#include "glogger.hpp"
#include "gsymbol.hpp"
#include <memory>
#include <vector>

class ggrammar;

/// parser state machine generator
class ggenerator {
  private:
    /// grammar
    std::shared_ptr<ggrammar> m_grammar;
    /// parser identifier
    std::string m_identifier;
    /// actions in the parser
    std::vector<std::shared_ptr<gaction>> m_actions;
    /// productions in the parser
    std::vector<std::shared_ptr<gproduction>> m_productions;
    /// symbols in the parser
    std::vector<std::shared_ptr<gsymbol>> m_symbols;
    /// start state
    std::shared_ptr<gsymbol> m_start_symbol;
    /// end state
    std::shared_ptr<gsymbol> m_end_symbol;
    /// error state
    std::shared_ptr<gsymbol> m_error_symbol;
    /// number of errors occurred during parsing and generator
    int m_errors;
    /// logger
    glogger m_log;

  public:
    ggenerator();

  public:
    int generate(const std::shared_ptr<ggrammar> &);
};
