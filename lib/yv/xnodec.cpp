#include "include/xnodec.hpp"
#include "include/xnode.hpp"

#include <cassert>

bool xnodec::operator()(const std::shared_ptr<xnode> &lhs, const std::shared_ptr<xnode> &rhs) const {
    assert(lhs.get());
    assert(rhs.get());
    return *lhs < *rhs;
}
