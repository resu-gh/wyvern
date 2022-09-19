#pragma once

#include <ostream>

/// symbol type
enum class gtranstype {
    /// shift a terminal symbol onto the parser's stack
    TRANSITION_SHIFT,
    /// reduce 1 or more symbols on the parser's stack into a non-terminal symbol
    TRANSITION_REDUCE,
};

inline std::ostream &operator<<(std::ostream &o, const gtranstype &t) {
    switch (t) {
    case gtranstype::TRANSITION_SHIFT:
        o << "TRSHIFT";
        break;
    case gtranstype::TRANSITION_REDUCE:
        o << "TRREDUCE";
        break;
    }
    return o;
}
