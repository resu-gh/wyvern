#pragma once

#include "ylogger.hpp"
#include "ytype.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

/// @brief cli flags/args parser

class yparser {
  private:
    using args = std::vector<std::string>;
    using arg_citer = args::const_iterator;
    using argst = std::vector<ytype>;
    args m_args;
    argst m_argst;
    ylogger m_log;

  private:
    bool m_help;
    std::string m_ifile;
    std::string m_ofile;

  public:
    yparser(int, char **);

  public:
    const std::string &ifile() const;
    const std::string &ofile() const;

  public:
    void panic(const std::string & = "");
    void parse();
    void check();
    void build();
    bool try_flag(arg_citer);
    bool try_arg();
};
