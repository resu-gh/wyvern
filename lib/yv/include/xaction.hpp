#pragma once

#include <string>

class xaction {
  private:
    /// action index
    int m_index;
    /// action identifier
    std::string m_identifier;

  public:
    xaction(int index, const std::string &identifier);

  public:
    int index() const;
    const std::string &identifier() const;

  public:
    static const int INVALID_INDEX = -1;
};
