#pragma once

#include <memory>

class gstate;

class gstatec {
  public:
    bool operator()(const std::shared_ptr<gstate> &lhs, const std::shared_ptr<gstate> &rhs) const;
};
