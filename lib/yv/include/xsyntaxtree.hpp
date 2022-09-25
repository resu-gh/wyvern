#pragma once

#include "glogger.hpp"
#include "xtoken.hpp"

#include <memory>
#include <vector>

class xgenerator;

class xsyntaxtree {
  private:
    ///
    std::shared_ptr<xgenerator> m_generator;
    /// logger
    glogger m_log;

  public:
    xsyntaxtree();

  public:
    void reset(const std::vector<xtoken> &tokens, const std::shared_ptr<xgenerator> &generator);
    void calculate_combined_parse_tree(const std::vector<xtoken> &tokens);

    void parse_regular_expression(const xtoken &token);
    void parse_literal(const xtoken &token);
};
