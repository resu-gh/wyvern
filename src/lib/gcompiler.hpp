#pragma once

#include "lib/ggrammar.hpp"
#include "lib/gparser.hpp"

#include <string>

class gcompiler {
  public:
    gcompiler();

public:
    int compile(const std::string::const_iterator&, const std::string::const_iterator&);
};
