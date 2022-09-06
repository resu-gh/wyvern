#include "include/yparser.hpp"

yparser::yparser(int argc, char **argv)
    : m_args(argv + 1, argv + argc),
      m_argst(),
      m_log(),
      m_help(false),
      m_ifile(),
      m_ofile() {
    parse();
    check();
    build();
}

const std::string &yparser::ifile() const {
    return m_ifile;
}

const std::string &yparser::ofile() const {
    return m_ofile;
}

void yparser::panic(const std::string &msg) {
    m_log.out << "wyvern - lalr parser generator\n";
    m_log.out << (!msg.empty() ? msg + "\n\n" : "\n");
    m_log.out << " -h           print this help message\n";
    m_log.out << " -i [file]    input grammar file [.g] (required)\n";
    m_log.out << " -o [file]    output file (c++ parser) (default: stdout)\n";
    std::exit(0);
}

bool yparser::try_flag(arg_citer arg) {
    if ((*arg)[0] != '-')
        return false;
    switch ((*arg)[1]) {
    case 'h':
        m_argst.push_back(ytype::HFLAG);
        m_help = true;
        return true;
    case 'o':
        m_argst.push_back(ytype::OFLAG);
        return true;
    case 'i':
        m_argst.push_back(ytype::IFLAG);
        return true;
    default:
        return false;
    }
}

bool yparser::try_arg() {
    switch (m_argst.back()) {
    case ytype::OFLAG:
        m_argst.push_back(ytype::OARGS);
        return true;
    case ytype::IFLAG:
        m_argst.push_back(ytype::IARGS);
        return true;
    default:
        return false;
    }
}

void yparser::parse() {
    arg_citer arg = m_args.begin();
    for (; arg != m_args.end() && !m_help; ++arg)
        if (!try_flag(arg) && !try_arg())
            panic(std::string("ERROR: unknown ") + *arg);
}

void yparser::check() {
    if (m_help)
        panic();
    std::set<ytype> argss(m_argst.begin(), m_argst.end());
    if (argss.size() != m_argst.size())
        panic("ERROR: found duplicates!");
    if (!std::count(m_argst.begin(), m_argst.end(), ytype::IFLAG))
        panic("ERROR: -i flag is required!");
    if (!std::count(m_argst.begin(), m_argst.end(), ytype::IARGS))
        panic("ERROR: -i arg is required!");
}

void yparser::build() {
    for (size_t i = 0; i < m_argst.size(); i++) {
        switch (m_argst.at(i)) {
        case ytype::OARGS:
            m_ofile = m_args.at(i);
            break;
        case ytype::IARGS:
            m_ifile = m_args.at(i);
            break;
        default:
            break;
        }
    }
}
