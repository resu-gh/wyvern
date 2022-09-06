#pragma once

#include <memory>
#include <string>

class ggrammar;

class gcompiler {
  private:
    std::shared_ptr<ggrammar> m_grammar;

  public:
    gcompiler();

  public:
    int compile(std::string::iterator &, std::string::iterator &);
};
