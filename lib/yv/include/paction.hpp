#pragma once

#include <string>

class paction {
  public:
    /// action index
    int index;
    /// action identifier
    std::string identifier;

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};