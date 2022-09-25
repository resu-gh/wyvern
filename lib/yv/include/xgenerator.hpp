#pragma once

#include "glogger.hpp"
#include "xsyntaxtree.hpp"
#include "xtoken.hpp"

#include <memory>
#include <vector>

class xgenerator : public std::enable_shared_from_this<xgenerator> {
  private:
    /// 
    std::shared_ptr<xsyntaxtree> m_sytax_tree;
    /// logger
    glogger m_log;

  public:
    xgenerator();

  public:
    std::shared_ptr<xgenerator> self();

  public:
    int generate(const std::vector<xtoken> &tokens);
};
