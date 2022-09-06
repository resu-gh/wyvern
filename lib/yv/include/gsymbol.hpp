#pragma once

/// grammar symbol
class gsymbol {
  private:
    /// symbol index (among all symbols)
    int m_index;

  public:
    gsymbol();

  public:
    int index() const;
};