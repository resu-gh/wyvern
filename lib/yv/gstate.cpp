#include "include/gstate.hpp"

gstate::gstate() // TODO INVALID_INDEX -> -1
    : m_index(INVALID_INDEX) {}

int gstate::index() const {
    return m_index;
}
