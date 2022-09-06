#pragma once

#include "gaction.hpp"
#include "gsymbolassoc.hpp"

#include <memory>
#include <string>
#include <vector>

class ggrammar {
  private:
    std::string m_identifier;
    std::vector<std::unique_ptr<gaction>> m_actions;
    bool m_active_whitespace_directive;
    bool m_active_precedence_directive;
    gsymbolassoc m_associativity;
    int m_precedence;

  public:
    ggrammar();

  public:
};
