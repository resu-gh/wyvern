#pragma once

#include "gsymboltype.hpp"

#include <string>

class psymbol {
  public:
    /// symbol index
    int index;
    /// symbol identifier
    std::string identifier;
    /// symbol lexeme
    std::string lexeme;
    /// symbol type
    gsymboltype type;
};