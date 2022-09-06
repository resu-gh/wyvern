#pragma once

#include "ylogger.hpp"

#include <fstream>
#include <iostream>

/// @brief io streams manager

class ystream {
  private:
    ylogger m_log;
    std::ifstream m_istream;

  public:
    std::ofstream out;
    std::string source;

  public:
    ystream(const std::string &ifile, const std::string &ofile);

  private:
    void try_read(const std::string &);
    void try_write(const std::string &);
};
