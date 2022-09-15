#include "include/gtransition.hpp"

gtransition::gtransition() // TODO INVALID_INDEX -> -1
    : m_index(INVALID_INDEX) {}

int gtransition::index() const {
    return m_index;
}
