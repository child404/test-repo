# default paths
BUILD_DIR = build
SRC_DIR = src
SOURCES = $(shell find $(SRC_DIR)/ -name "*.cxx")
TARGET  = program
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cxx=$(BUILD_DIR)/%.o)

# define flags and libs
ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

# add xml files
ROOTLIBS += -lMLP -lXMLIO

# add TSpectrum
ROOTLIBS += -lSpectrum

OUTLIB = ./obj/
CXX  = g++
CXX += -I./     

CXXFLAGS  = -g -Wall -fPIC -Wno-deprecated
CXXFLAGS += $(ROOTCFLAGS)
CXXFLAGS += $(ROOTLIBS)
CXXFLAGS += $(ROOTGLIBS)

PATHTOSHAREDLIB=$(OUTLIB)

#vpath %.cxx src
#vpath %.h include

#----------------------------------------------------#

default: $(TARGET)

.SECONDEXPANSION:

$(OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.cxx,$$@)
        mkdir -p $(@D)
        $(CXX) -c -o $@ $(CXXFLAGS) $<

$(TARGET): $(OBJECTS)
        $(CXX) -o $@ $(CXXFLAGS) $^

.PHONY: default

#all: main
#
#main: main.cxx
#	$(CXX) -o $@ $^ $(CXXFLAGS)
#test: test.cpp
#	$(CXX) -o $@ $^ $(CXXFLAGS)
#  
#
#clean: 
#	rm -f *~
#	rm -f main
