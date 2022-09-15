#pragma once

class gstate {
  private:
    /// state index
    int m_index;

  public:
    gstate();

  public:
    int index() const;

    static const int INVALID_INDEX = -1; // TODO maybe useless
};
