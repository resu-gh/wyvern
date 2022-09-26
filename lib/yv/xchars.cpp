#include "include/xchars.hpp"
#include <cassert>

xchars::xchars(int begin_char, int end_char)
    : m_begin_char(begin_char),
      m_end_char(end_char) {
    assert(m_begin_char < m_end_char);
}

int xchars::begin_char() const {
    return m_begin_char;
}

int xchars::end_char() const {
    return m_end_char;
}

bool xchars::operator<(const xchars &regex_character) const {
    return m_end_char < regex_character.m_begin_char;
}