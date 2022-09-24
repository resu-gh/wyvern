#pragma once

#include "glogger.hpp"

#include <string>

class paction {
  public:
    /// action index
    int index;
    /// action identifier
    const char* identifier;

  private:
    /// logger
    glogger m_log;

  public:
    paction();

  public:
    void json(int sc, bool nested, int in, bool inlined, int uc = 0) const;

  public:
    static const int INVALID_INDEX = -1; // TODO maybe useless
};
