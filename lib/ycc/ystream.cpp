#include "include/ystream.hpp"

#include <algorithm>

ystream::ystream(const std::string &ifile, const std::string &ofile)
    : m_log(),
      source() {
    try_read(ifile);
    try_write(ofile);
}

void ystream::try_read(const std::string &ifile) {
    m_istream.open(ifile);
    if (m_istream.fail()) {
        m_log.out << m_log.cred << "[ERROR] ";
        m_log.out << m_log.creset << "unable to open input file ";
        m_log.out << m_log.cmagenta << ifile;
        m_log.out << m_log.creset << "\n";
        std::exit(0);
    }
    m_istream.seekg(0, std::ios::end);
    source.reserve(m_istream.tellg());
    m_istream.seekg(0, std::ios::beg);
    using sbuf_iter = std::istreambuf_iterator<char>;
    source.assign(sbuf_iter(m_istream), sbuf_iter());
}

void ystream::try_write(const std::string &ofile) {
    if (!ofile.empty()) {
        out.open(ofile);
        m_log.out << m_log.cblue << "[INFO] ";
        m_log.out << m_log.creset << "writing to ";
        m_log.out << m_log.cmagenta << ofile;
        m_log.out << m_log.creset << "\n";
    } else {
        out.basic_ios<char>::rdbuf(std::cout.rdbuf());
        m_log.out << m_log.cblue << "[INFO] ";
        m_log.out << m_log.creset << "writing to ";
        m_log.out << m_log.cmagenta << "stdout";
        m_log.out << m_log.creset << "\n";
    }
}

std::string ystream::sanitize(const std::string &input) {
    std::size_t length = input.size();
    std::size_t backsl = std::count(input.begin(), input.end(), '\\');
    std::string output;
    output.reserve(length + backsl);
    for (std::string::const_iterator i = input.begin(); i != input.end(); ++i) {
        char character = *i;
        if (character == '\\' || character == '"')
            output.push_back('\\');
        output.push_back(character);
    }
    return output;
}
