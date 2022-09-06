#pragma once

/// production entity
/// defines a symbol on the lhs that is reduced
// from 0 or more nodes on the rhs
class gproduction {
  private:
    /// production index
    int m_index;

  public:
    gproduction(int index); // TODO FIXME

  public:
    int index() const;
};