#pragma once

#include "ylogger.hpp"

#include <fstream>
#include <iostream>
#include <utility>

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

  public:
    std::string sanitize(const std::string &input);
    template <typename... T>
    std::string fmt(const std::string &format, T &&...args);

  private:
    void try_read(const std::string &);
    void try_write(const std::string &);
};

template <typename... T>
std::string ystream::fmt(const std::string &format, T &&...args) {
    auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<T>(args)...);
    std::string output(size + 1, '\0');
    std::sprintf(&output[0], format.c_str(), std::forward<T>(args)...);
    return output.c_str();
}
