#pragma once

class gitem {
  private:
    /// the position of the `dot` (cursor) in this item
    int m_position;

  public:
    gitem();

  public:
    int position() const;
};
