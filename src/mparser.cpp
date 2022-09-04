#include "include/mparser.hpp"

mparser::mparser(int argc, char **argv)
    : m_args(argv + 1, argv + argc),
      m_argst(),
      m_help(false),
      m_ifile(),
      m_ofile(),
      m_log() {
    parse();
    check();
    build();
}

const std::string &mparser::ifile() const {
    return m_ifile;
}

const std::string &mparser::ofile() const {
    return m_ofile;
}

void mparser::panic(const std::string &msg) {
    m_log.out << "wyvern - lalr parser generator\n";
    m_log.out << (!msg.empty() ? msg + "\n\n" : "\n");
    m_log.out << " -h           print this help message\n";
    m_log.out << " -i [file]    input grammar file [.g] (required)\n";
    m_log.out << " -o [file]    output file (c++ parser) (default: stdout)\n";
    std::exit(0);
}

bool mparser::try_flag(arg_citer arg) {
    if ((*arg)[0] != '-')
        return false;
    switch ((*arg)[1]) {
    case 'h':
        m_argst.push_back(mtype::HFLAG);
        m_help = true;
        return true;
    case 'o':
        m_argst.push_back(mtype::OFLAG);
        return true;
    case 'i':
        m_argst.push_back(mtype::IFLAG);
        return true;
    default:
        return false;
    }
}

bool mparser::try_arg(arg_citer arg) {
    switch (m_argst.back()) {
    case mtype::OFLAG:
        m_argst.push_back(mtype::OARGS);
        return true;
    case mtype::IFLAG:
        m_argst.push_back(mtype::IARGS);
        return true;
    default:
        return false;
    }
}

void mparser::parse() {
    arg_citer arg = m_args.begin();
    for (; arg != m_args.end() && !m_help; ++arg)
        if (!try_flag(arg) && !try_arg(arg))
            panic(std::string("ERROR: unknown ") + *arg);
}

void mparser::check() {
    if (m_help)
        panic();
    std::set<mtype> argss(m_argst.begin(), m_argst.end());
    if (argss.size() != m_argst.size())
        panic("ERROR: found duplicates!");
    if (!std::count(m_argst.begin(), m_argst.end(), mtype::IFLAG))
        panic("ERROR: -i flag is required!");
    if (!std::count(m_argst.begin(), m_argst.end(), mtype::IARGS))
        panic("ERROR: -i arg is required!");
}

void mparser::build() {
    for (size_t i = 0; i < m_argst.size(); i++) {
        switch (m_argst.at(i)) {
        case mtype::OARGS:
            m_ofile = m_args.at(i);
            break;
        case mtype::IARGS:
            m_ifile = m_args.at(i);
            break;
        default:
            break;
        }
    }
}
