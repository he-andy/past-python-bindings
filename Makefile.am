#
# Makefile.am: This file is part of the PAST project.
#
# PAST: the PoCC Abstract Syntax Tree
#
# Copyright (C) 2014 the Ohio State University
#
# This program can be redistributed and/or modified under the terms
# of the license specified in the LICENSE.txt file at the root of the
# project.
#
# Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
# Author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
#
#

#############################################################################
SUBDIRS			=

#############################################################################
MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_builddir) -I$(top_builddir)/past	\
			-I$(top_srcdir) -I$(top_srcdir)/past		\
			-I$(install_prefix) -$(install_prefix)/past	\
			-I$(top_srcdir)/past/include \
			-I$(top_srcdir)/past/include/past \
			-I$(shell python3 -m pybind11 --includes)

# Python module build rules
pybind11_module_name = past
pybind11_module_SOURCES = bindings.cpp
pybind11_module_CXXFLAGS = -fPIC $(shell python3-config --cflags)
pybind11_module_LDFLAGS = -shared $(shell python3-config --ldflags)

# Rule to build the Python module shared library
$(pybind11_module_name).so: $(pybind11_module_SOURCES)
	$(CXX) $(pybind11_module_CXXFLAGS) $(pybind11_module_SOURCES) -I${INCLUDES} -o $(pybind11_module_name).so $(pybind11_module_LDFLAGS)

# Ensure the Python module is built when calling 'all'
all-local: $(pybind11_module_name).so

# Add the shared object file to the list of files to clean
clean-local:
	rm -f $(pybind11_module_name).so $(pybind11_module_OBJECTS)

#############################################################################

EXTRA_DIST		= past-equivalence

noinst_HEADERS		= past.h

bin_PROGRAMS		= pastchecker

pastchecker_SOURCES	= past.c

pastchecker_LDADD	= $(top_srcdir)/past/libpast.la
