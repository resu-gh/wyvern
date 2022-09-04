#pragma once

#include <string>

class ggrammar;

class gparser {
  public:
    gparser();

  public:
    int parse(const std::string::const_iterator, const std::string::const_iterator, ggrammar *);
};
