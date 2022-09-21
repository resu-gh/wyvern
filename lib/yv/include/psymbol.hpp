#pragma once

#include "glogger.hpp"
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

  private:
    /// logger
    glogger m_log;

  public:
    psymbol();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;
};
