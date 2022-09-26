#pragma once

class xchars {
  private:
    /// the first char in the range of chars
    int m_begin_char;
    /// one past the last char in the range of chars
    int m_end_char;

  public:
    xchars(int begin_char, int end_char);

  public:
    int begin_char() const;
    int end_char() const;

    bool operator<(const xchars &regex_character) const;
};