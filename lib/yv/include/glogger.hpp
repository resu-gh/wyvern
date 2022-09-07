#pragma once

#include <iostream>

/// @brief generic logger

class glogger final {

  public:
    std::ostream &out;
    std::ostream &err;

    const std::string creset;
    const std::string cred;
    const std::string cgreen;
    const std::string cyellow;
    const std::string cblue;
    const std::string cmagenta;
    const std::string ccyan;
    const std::string cwhite;

    const std::string cerror;
    const std::string cgcomp;
    const std::string cggram;
    const std::string cgpars;
    const std::string cggenr;

  public:
    glogger();
};
