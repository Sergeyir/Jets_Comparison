# Makefile is a part of the PYTHIA event generator.

# Set the shell.
SHELL=/usr/bin/env bash

# Include the configuration.
-include Makefile.inc

# Check distribution (use local version first, then installed version).
ifneq ("$(wildcard ../lib/libpythia8.*)","")
  PREFIX_LIB=../lib
  PREFIX_INCLUDE=../include
endif
CXX_COMMON:=-I$(PREFIX_INCLUDE) $(CXX_COMMON) $(GZIP_LIB)
CXX_COMMON+= -L$(PREFIX_LIB) -Wl,-rpath,$(PREFIX_LIB) -lpythia8 -ldl
PYTHIA=$(PREFIX_LIB)/libpythia8$(LIB_SUFFIX)

# Fix missing runtime paths from RIVET.
ifeq ($(RIVET_USE),true)
  COMMA=,
  RIVET_LPATH=$(filter -L%,$(shell $(RIVET_BIN)$(RIVET_CONFIG) --ldflags))
  RIVET_RPATH=$(subst -L,-Wl$(COMMA)-rpath=,$(RIVET_LPATH))
endif

################################################################################
# RULES: Definition of the rules used to build the PYTHIA examples.
################################################################################

# Rules without physical targets (secondary expansion for specific rules).
.SECONDEXPANSION:
.PHONY: all clean

# All targets (no default behavior).
all:
	$(info Usage: make pythia_jets)

# PYTHIA library.
$(PYTHIA):
	$(error Error: PYTHIA must be built, please run "make"\
                in the top PYTHIA directory)
	
pythia_jets: $(PYTHIA) pythia_jets.cc
ifeq ($(FASTJET3_USE), true)
	$(CXX) $@.cc -o $@ $(FASTJET3_INCLUDE) -w $(CXX_COMMON) $(FASTJET3_LIB)
else
	$(error Error: $@ requires FASTJET3)
endif
	
herwig_jets: herwig_jets.cc
ifeq ($(FASTJET3_USE), true)
	$(CXX) $@.cc -o $@ $(FASTJET3_INCLUDE) -w $(CXX_COMMON) $(FASTJET3_LIB)
else
	$(error Error: $@ requires FASTJET3)
endif

# Clean.
clean:
	rm -f pythia_jets;|\
	rm -f *~; rm -f \#*; rm -f core*; rm -f *Dct.*; rm -f *.so;\
	rm -f *.log;

