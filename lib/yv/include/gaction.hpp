#pragma once

#include <string>

/// action attached to a parser reduction
class gaction {
  private:
    int m_index;
    std::string m_identifier;

  public:
    gaction(int index, const std::string &identifier);

  public:
    int index() const;
    const std::string &identifier() const;

  public:
    static const int INVALID_INDEX = -1;
};