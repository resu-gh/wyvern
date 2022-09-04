#pragma once

#include "lib/include/ggrammar.hpp"

#include <iostream>
#include <memory>

class ggenerator {
  public:
    ggenerator();

  public:
    int generate(const std::shared_ptr<ggrammar> &);
};
