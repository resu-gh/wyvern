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

  private:
    void calculate_identifiers();
    void check_for_undefined_symbol_errors();
    void check_for_unreferenced_symbol_errors();
    void check_for_error_symbol_on_left_hand_side_errors();
    void calculate_terminal_and_non_terminal_symbols();
    void calculate_implicit_terminal_symbols();
    void replace_references_to_symbol(const std::shared_ptr<gsymbol> &to_symbol, const std::shared_ptr<gsymbol> &with_symbol);

  public:
    void dump() const;
};
