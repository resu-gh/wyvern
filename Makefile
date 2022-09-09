SRCD := src lib
SRCS := $(shell find $(SRCD) -name "*.cpp" -or -name "*.hpp")

BAZELCC := clang++
BAZELBIN := bazelisk # bazel

ARGS := -i $(shell pwd)/unit/grammar/errcalc.g

.SILENT: run
.PHONY: run
run:
	CC=$(BAZELCC) $(BAZELBIN) run //src:ycc -- $(ARGS)

.SILENT: bld
.PHONY: bld
bld:
	CC=$(BAZELCC) $(BAZELBIN) build //src:ycc

.SILENT: test
.PHONY: test
test:
	CC=$(BAZELCC) $(BAZELBIN) test --test_output=errors //unit:yvu

.SILENT: clean
.PHONY: clean
clean:
	$(BAZELBIN) clean

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
