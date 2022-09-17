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

    const std::string cnr;
    const std::string chl;

    uint32_t sep;
    uint32_t osep;

  private:
    const std::string m_mod;
    const std::string m_loc;
    std::string m_fun;
    const std::string m_col;

  public:
    glogger(const std::string &, const std::string &, int = 0);

  public:
    void set_fun(const std::string &);

  public:
    std::string sp(int) const;
    std::string fg(int) const;
    std::string bg(int) const;
    std::string op(const std::string &) const;
    std::ostream &trace(int, std::ostream & = std::cout) const;
    std::ostream &etrace(int) const;
};
