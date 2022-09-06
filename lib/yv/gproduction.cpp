#include "include/gproduction.hpp"

gproduction::gproduction(int index)
    : m_index(index) {}

int gproduction::index() const {
    return m_index;
}