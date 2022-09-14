#pragma once

#include <memory>

class gsymbol;

class gsymbolc {
  public:
    bool operator()(const std::shared_ptr<gsymbol> &lhs, const std::shared_ptr<gsymbol> &rhs) const;
};
