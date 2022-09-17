#include "include/gstatec.hpp"
#include "include/gstate.hpp"

bool gstatec::operator()(const std::shared_ptr<gstate> &lhs, const std::shared_ptr<gstate> &rhs) const {
    return *lhs < *rhs;
}
