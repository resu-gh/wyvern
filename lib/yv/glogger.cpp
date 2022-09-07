#include "include/glogger.hpp"

glogger::glogger()
    : out(std::cout),
      err(std::cerr),
      creset("\033[0m"),
      cred("\033[31m"),
      cgreen("\033[32m"),
      cyellow("\033[33m"),
      cblue("\033[34m"),
      cmagenta("\033[35m"),
      ccyan("\033[36m"),
      cwhite("\033[37m"),
      cerror("\033[48;5;89m"),
      cgcomp("\033[38;5;45m"),
      cggram("\033[38;5;135m"),
      cgpars("\033[38;5;214m"),
      cggenr("\033[38;5;112m") {}
