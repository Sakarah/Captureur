# -*- Makefile -*-

lib_TARGETS = champion

# Tu peux rajouter des fichiers sources, headers, ou changer
# des flags de compilation.
champion-srcs = prologin.cc map.cc
champion-dists = map.hh
champion-cxxflags = -std=c++11 -Wall -Wextra -O2

# Evite de toucher a ce qui suit
champion-dists += prologin.hh
STECHEC_LANG=cxx
include ../includes/rules.mk
