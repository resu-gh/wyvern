#pragma once

#include "glogger.hpp"

#include <string>

/// action attached to a parser reduction
class gaction {
  private:
    /// action index
    int m_index;
    /// action identifier
    std::string m_identifier;
    /// logger
    glogger m_log;

  public:
    gaction(int index, const std::string &identifier);

  public:
    int index() const;
    const std::string &identifier() const;

  public:
    std::string microdump() const;

  public:
    static const int INVALID_INDEX = -1;
};
