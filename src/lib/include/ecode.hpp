#pragma once

#include <ostream>

enum class ecode {
    E_SYNTAX,
};

inline std::ostream &operator<<(std::ostream &o, const ecode &t) {
    switch (t) {
    case ecode::E_SYNTAX:
        return o << "E_SYNTAX";
    }
}