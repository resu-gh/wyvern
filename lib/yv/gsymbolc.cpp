#include "include/gsymbolc.hpp"
#include "include/gsymbol.hpp"

#include <cassert>

bool gsymbolc::operator()(const std::shared_ptr<gsymbol> &lhs, const std::shared_ptr<gsymbol> &rhs) const {
    assert(lhs->index() >= 0);
    assert(rhs->index() >= 0);
    return lhs->index() < rhs->index();
}
