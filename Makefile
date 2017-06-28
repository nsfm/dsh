
# Binary name.
OUTPUT_NAME := dsh

# The compilation is pretty simple, so this is the only tool I'm specifying.
# For most cross compilation cases, simply `export CXX="/path/to/arm-linux-gcc"`
# or such before you `make`.
CXX ?= g++

# Working directories, to keep the base directory clean.
BUILDDIR := build
DEPDIR   := dependencies
SRCDIR   := src

# Optional parameters here, so they can be overridden.
CXXFLAGS ?= -Wall -Wextra -fexceptions -Ofast -fdiagnostics-color
LDFLAGS  ?= -fdiagnostics-color
DEFINES  ?=
INCLUDES ?=

# Parameters not intended to be overridden added here.
CXXFLAGS += -pthread -std=c++11
DEFINES  += -DCOMPILEDBY='"$(shell whoami)"' -DCOMPILEDDATE='"$(shell date)"'
LDFLAGS  += -lstdc++ -lpthread
INCLUDES += $(SRCDIR)

# We'll be using GCC to generate dependency files.
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

# All .cc files in the SRCDIR are compiled and linked into this binary.
SRCS := $(notdir $(wildcard $(SRCDIR)/*.cc))
OBJS := $(addprefix $(BUILDDIR)/,$(SRCS:.cc=.o))

.PHONY: all clean
.PRECIOUS: $(DEPDIR)/%.d

all: $(OUTPUT_NAME)
	@echo "Done!"
	@echo

clean:
	@echo "Cleaning up..."
	rm -f $(OUTPUT_NAME)
	rm -rf $(BUILDDIR) $(DEPDIR)

$(BUILDDIR) $(DEPDIR):
	@echo "Creating $@ directory..."
	mkdir -p $@
	@echo

# One rule to build when dependency files are available - one for without.
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc | $(BUILDDIR) $(DEPDIR)
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc $(DEPDIR)/%.d | $(BUILDDIR) $(DEPDIR)
	@echo "Compiling $<..."
	$(CXX) -c $(CXXFLAGS) $(DEPFLAGS) $(addprefix -I,$(INCLUDES)) $(DEFINES) -o $@ $<
	@echo
	@echo "Saving dependency file..."
	mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
	@echo "Compiled $<!"
	@echo

$(OUTPUT_NAME): $(OBJS)
	@echo "Linking $@..."
	$(CXX) -o $@ $^ $(LDFLAGS)
	@echo "Linked successfully!"
	@echo

# Empty rule to avoid the warning (dependency generation is a byproduct of the compilation).
$(DEPDIR)/%.d: ;

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
