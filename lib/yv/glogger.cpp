#include "include/glogger.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

glogger::glogger(const std::string &loc, int col)
    : m_loc("yyv:" + loc + ":"),
      m_col(fg(col)) {}

void glogger::set_function(const std::string &fun) {
    m_fun = fun + "()";
}

std::string glogger::fg(int col) const {
    if (col < 0 || col > 255)
        throw std::runtime_error("invalid ansi escape code\n");
    std::stringstream s;
    s << "\033[38;5;";
    s << std::to_string(col);
    s << "m";
    return s.str();
}

std::string glogger::bg(int col) const {
    if (col < 0 || col > 255)
        throw std::runtime_error("invalid ansi escape code\n");
    std::stringstream s;
    s << "\033[48;5;";
    s << std::to_string(col);
    s << "m";
    return s.str();
}

std::ostream &glogger::trace(int cont) const {
    std::stringstream s;
    if (cont)
        s << std::string(sep, ' ');
    else
        s << m_loc << m_fun;
    if (s.str().size() < sep)
        s << std::string(sep - s.str().size(), ' ');
    return out << m_col << s.str() << cnr;
}

std::ostream &glogger::etrace(int cont) const {
    std::stringstream s;
    if (cont)
        s << std::string(sep, ' ');
    else
        s << m_loc << m_fun;
    if (s.str().size() < sep)
        s << std::string(sep - s.str().size(), ' ');
    return err << m_col << s.str() << cred;
}

std::string glogger::op(const std::string &op) const {
    std::stringstream s;
    if (op.size() < osep)
        s << std::string(osep - op.size(), ' ');
    s << cmagenta << op << " " << cnr;
    return s.str();
}
