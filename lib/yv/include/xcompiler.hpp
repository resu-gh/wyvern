#pragma once

#include "glogger.hpp"
#include "lstatemachine.hpp"
#include "xgenerator.hpp"
#include "xtoken.hpp"

#include <memory>
#include <vector>

class xcompiler {
  private:
    /// allocated state machine
    std::unique_ptr<lstatemachine> m_state_machine;
    /// logger
    glogger m_log;

  public:
    xcompiler();

  public:
    const std::unique_ptr<lstatemachine> &state_machine() const;

  public:
    void compile(const std::vector<std::shared_ptr<xtoken>> &tokens);
    void populate_lexer_state_machine(const std::shared_ptr<xgenerator> &generator);
};
