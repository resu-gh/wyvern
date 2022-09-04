TARGET := wyvern

PREFIX := 
DESTDIR := 

BUILDDIR = build
SRCDIRS = src

SRCS := $(shell find $(SRCDIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILDDIR)/%.o)
DEPS := $(OBJS:.o=.d)

INCDIRS := $(shell find $(SRCDIRS) -type d)
INCFLAGS := $(addprefix -I,$(INCDIRS))

CC := clang++
CPPFLAGS := $(INCFLAGS) -MMD -MP
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++17 -g

LDFLAGS :=

ARGS := -i ./unit/grammars/json.g

.PHONY: run
run: clean $(BUILDDIR)/$(TARGET)
	./$(BUILDDIR)/$(TARGET) $(ARGS)

$(BUILDDIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILDDIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*

FILES := $(shell find $(SRCDIRS) -name "*.cpp" -or -name "*.hpp")

.SILENT: tidy
.PHONY: tidy
tidy:
	for file in $(FILES); do \
		echo "clang-tidy $$file --warnings-as-errors='*'"; \
		clang-tidy $$file --warnings-as-errors="*" ; \
	done

.SILENT: fmt
.PHONY: fmt
fmt:
	for file in $(FILES); do \
		echo "clang-format $$file -i"; \
		clang-format $$file -i ; \
	done

-include $(DEPS)
