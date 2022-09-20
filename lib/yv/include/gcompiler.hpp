#pragma once

#include "ggenerator.hpp"
#include "glogger.hpp"
#include "paction.hpp"
#include "pstate.hpp"
#include "psymbol.hpp"

#include <array>
#include <deque>
#include <memory>
#include <string>
#include <vector>

class ggrammar;

class gcompiler {
  private:
    /// grammar
    std::shared_ptr<ggrammar> m_grammar;
    /// generator
    std::shared_ptr<ggenerator> m_generator;

    /// test
    std::deque<std::string> m_strings;
    std::unique_ptr<std::vector<paction>> m_actions;
    std::unique_ptr<std::vector<psymbol>> m_symbols;
    std::unique_ptr<std::vector<pstate>> m_states;
    /// test

    /// logger
    glogger m_log;

  public:
    gcompiler();

  public:
    int compile(std::string::iterator &, std::string::iterator &);

  private:
    const std::string &add_string(const std::string &string);
    void populate_parser_state_machine();

    /// debug
  public:
    ~gcompiler() {
        std::cout << "\033[0mgcompiler " << &*this << " destroyed\n";
    }
};
