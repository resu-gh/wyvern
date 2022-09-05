SRCD := src libwyv libwyvcc

SRCS := $(shell find $(SRCD) -name "*.cpp" -or -name "*.hpp")

.SILENT: fmt
.PHONY: fmt
.ONESHELL:
fmt:
	for file in $(SRCS); do \
		echo "clang-format $$file -i"; \
		clang-format $$file -i ; \
	done

.SILENT: tidy
.PHONY: tidy
.ONESHELL:
tidy:
	echo "requires compile_commands.json (todo)"
	exit
	for file in $(SRCS); do \
		echo "clang-tidy $$file --warnings-as-errors='*'"; \
		clang-tidy $$file --warnings-as-errors="*" ; \
	done
