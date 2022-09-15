#pragma once

class gtransition {
  private:
    /// transition index
    mutable int m_index;

  public:
    gtransition();

  public:
    int index() const;

    static const int INVALID_INDEX = -1; // TODO maybe useless
};
