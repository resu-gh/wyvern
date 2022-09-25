#include "include/xcompiler.hpp"
#include "include/lstatemachine.hpp"
#include "include/xgenerator.hpp"

#include <iostream>
#include <memory>

xcompiler::xcompiler()
    : m_state_machine(std::make_unique<lstatemachine>()),
      m_log("yyv", "xcomp", 255) {}

const std::unique_ptr<lstatemachine> &xcompiler::state_machine() const {
    return m_state_machine;
}

void xcompiler::compile(const std::vector<xtoken> &tokens) {
    /*debug*/ std::string h = m_log.hook("compile");

    std::shared_ptr<xgenerator> generator = std::make_shared<xgenerator>();
    /*debug*/ m_log.htrace(h, "generator: ") << &*generator << "\n";

    int errors = generator->generate(tokens);

    if (errors == 0)
        populate_lexer_state_machine(generator);
}

void xcompiler::populate_lexer_state_machine(const std::shared_ptr<xgenerator> &generator) {
    /*debug*/ std::string h = m_log.hook("build_scanner_au");
    /*debug*/ m_log.htrace(h, "m_generator") << &*generator << "\n";
}
