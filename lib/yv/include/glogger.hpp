#pragma once

#include <iostream>

/// @brief generic logger

class glogger final {
  public:
    std::ostream &out = std::cout;
    std::ostream &err = std::cerr;

    const std::string creset = "\033[0m";
    const std::string cred = "\033[31m";
    const std::string cgreen = "\033[32m";
    const std::string cyellow = "\033[33m";
    const std::string cblue = "\033[34m";
    const std::string cmagenta = "\033[35m";
    const std::string ccyan = "\033[36m";
    const std::string cwhite = "\033[37m";

    const std::string cerror = "\033[48;5;89m";
    const std::string cgcomp = "\033[38;5;45m";
    const std::string cggram = "\033[38;5;135m";
    const std::string cgpars = "\033[38;5;214m";
    const std::string cggenr = "\033[38;5;112m";

    const std::string cnr = "\033[38;5;240m";
    const std::string chl = "\033[38;5;245m";

    uint32_t sep = 25;
    uint32_t osep = 6;

  private:
    std::string m_loc;
    std::string m_fun;
    std::string m_col;

  public:
    glogger(const std::string & = "", int = 0);

  public:
    void set_function(const std::string &);

  public:
    std::string fg(int) const;
    std::string bg(int) const;
    std::string op(const std::string&) const;
    std::ostream &trace(int) const;
    std::ostream &etrace(int) const;
};
