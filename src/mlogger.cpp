#include "include/mlogger.hpp"

mlogger::mlogger()
    : out(std::cout),
      err(std::cerr),
      creset("\033[0m"),
      cred("\033[31m"),
      cgreen("\033[32m"),
      cyellow("\033[33m"),
      cblue("\033[34m"),
      cmagenta("\033[35m"),
      ccyan("\033[36m"),
      cwhite("\033[37m") {}
