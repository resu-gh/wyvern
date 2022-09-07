#pragma once

#include "glogger.hpp"
#include <memory>

class ggrammar;

/// parser state machine generator
class ggenerator {
  private:
    std::shared_ptr<ggrammar> m_grammar;
    /// logger
    glogger m_log;

  public:
    ggenerator();

  public:
    int generate(const std::shared_ptr<ggrammar> &);
};
