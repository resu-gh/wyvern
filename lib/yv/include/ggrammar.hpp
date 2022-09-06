#pragma once

#include "gproduction.hpp"
#include "gsymbolassoc.hpp"

#include <memory>
#include <string>

class gsymbol;
class gproduction;

class ggrammar {
  private:
    /// grammar name
    std::string m_identifier;
    /// true iff a whitespace directive is active
    bool m_active_whitespace_directive;
    /// true iff a precedence directive is active
    bool m_active_precedence_directive;
    /// most recently set associativity
    gsymbolassoc m_associativity;
    /// currently active production
    std::shared_ptr<gproduction> m_active_production;
    /// currently active symbol
    std::shared_ptr<gsymbol> m_active_symbol;

  public:
    ggrammar();

  public:
    const std::string &identifier() const;

  public:
    // why ggrammar& returns ?
    ggrammar &grammar(const std::string &identifier);
    ggrammar &whitespace();
};
