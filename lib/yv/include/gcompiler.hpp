#pragma once

#include "ggenerator.hpp"
#include "ggrammar.hpp"
#include "gparser.hpp"

#include <memory>
#include <string>

class gcompiler {
  private:
    std::shared_ptr<ggrammar> m_grammar;

  public:
    gcompiler();

  public:
    int compile(std::string::iterator &, std::string::iterator &);
};
