#pragma once

#include "glogger.hpp"

#include <memory>
#include <string>

class ggrammar;

class gcompiler {
  private:
    /// grammar
    std::shared_ptr<ggrammar> m_grammar;
    /// logger
    glogger m_log;

  public:
    gcompiler();

  public:
    int compile(std::string::iterator &, std::string::iterator &);
};
