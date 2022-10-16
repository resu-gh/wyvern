#pragma once

#include <memory>

class xnode;

class xnodec {
  public:
    bool operator()(const std::shared_ptr<xnode> &lhs, const std::shared_ptr<xnode> &rhs) const;
};
