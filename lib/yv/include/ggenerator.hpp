#pragma once

#include <memory>

class ggrammar;

/// parser state machine generator
class ggenerator {
  public:
    ggenerator();

  public:
    int generate(const std::shared_ptr<ggrammar> &);
};
