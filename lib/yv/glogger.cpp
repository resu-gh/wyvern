#include "include/glogger.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

glogger::glogger(const std::string &mod, const std::string &loc, int col)
    : out(std::cout),
      err(std::cerr),
      creset(fg(0)),
      cred(fg(1)),
      cgreen(fg(2)),
      cyellow(fg(3)),
      cblue(fg(4)),
      cmagenta(fg(5)),
      ccyan(fg(6)),
      cwhite(fg(7)),
      cnr(fg(240)),
      chl(fg(245)),
      sep(50),
      osep(25),
      m_mod(mod),
      m_loc(mod + ":" + loc + ":"),
      m_fun(),
      m_col(fg(col)) {}

// --------------------

void glogger::set_fun(const std::string &fun) {
    m_fun = fun + "()";
}

std::ostream &glogger::trace(int cont, std::ostream &o) const {
    std::stringstream s;
    if (cont)
        s << sp(osep);
    else
        s << m_loc << m_fun << "\n";
    if (s.str().size() < sep)
        s << std::string(sep - s.str().size(), ' ');
    return o << m_col << s.str() << cnr;
}

std::ostream &glogger::etrace(int cont) const {
    return trace(cont, err);
}

// --------------------

std::string glogger::sp(int s) const {
    return std::string(s, ' ');
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

std::string glogger::op(const std::string &op) const {
    std::stringstream s;
    s << cmagenta << op;
    if (op.size() < osep)
        s << sp(osep - op.size());
    s << " " << cnr;
    return s.str();
}

std::string glogger::hook(const std::string &fn) const {
    return m_loc + fn + "()";
}

std::ostream &glogger::htrace(const std::string &hook, const std::string &oper) const {
    std::stringstream s;
    s << m_col << hook << "\n";
    s << cmagenta << op(oper) << cnr;
    return out << s.str();
}

std::ostream &glogger::ehtrace(const std::string &hook, const std::string &oper) const {
    std::stringstream s;
    s << m_col << hook << "\n";
    s << cmagenta << op(oper) << cred;
    return err << s.str();
}
