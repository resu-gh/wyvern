#pragma once

#include "glogger.hpp"
#include "xaction.hpp"
#include "xsyntaxtree.hpp"
#include "xtoken.hpp"

#include <memory>
#include <vector>

class xgenerator : public std::enable_shared_from_this<xgenerator> {
  private:
    /// the syntax tree generated from parsing regular expression(s)
    std::shared_ptr<xsyntaxtree> m_sytax_tree;
    /// the lexical analyzer actions
    std::vector<std::shared_ptr<xaction>> m_actions;
    /// logger
    glogger m_log;

  public:
    xgenerator();

  public:
    std::shared_ptr<xgenerator> self();

  public:
    int generate(const std::vector<std::shared_ptr<xtoken>> &tokens);

  public:
    const std::shared_ptr<xaction> &add_lexer_action(const std::string &identifier);
};
