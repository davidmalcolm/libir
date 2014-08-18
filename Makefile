#   Copyright 2014 David Malcolm <dmalcolm@redhat.com>
#   Copyright 2014 Red Hat, Inc.
#
#   This is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see
#   <http://www.gnu.org/licenses/>.

LIBIR_SO		:= libir.so
LIBIR_GCC_PLUGIN_SO	:= libir-gcc-plugin.so

GCCPLUGINS_DIR:= $(shell $(CC) --print-file-name=plugin)
CPPFLAGS+= -I$(GCCPLUGINS_DIR)/include -I$(GCCPLUGINS_DIR)/include/c-family -I.
CFLAGS+= -fPIC -g
CXXFLAGS+= -fPIC -g

all: run-test-plugin $(LIBIR_SO) $(LIBIR_GCC_PLUGIN_SO) test-plugin.so

LIBIR_CC_FILES := libir.cc
LIBIR_GCC_PLUGIN_CC_FILES := libir-gcc-plugin.cc

LIBIR_OBJECT_FILES := $(patsubst %.cc,%.o,$(LIBIR_CC_FILES))
LIBIR_GCC_PLUGIN_OBJECT_FILES := $(patsubst %.cc,%.o,$(LIBIR_GCC_PLUGIN_CC_FILES))

$(LIBIR_SO): $(LIBIR_OBJECT_FILES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -shared $^ -o $@ $(LIBS)

$(LIBIR_GCC_PLUGIN_SO): $(LIBIR_GCC_PLUGIN_OBJECT_FILES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -shared $^ -o $@ $(LIBS)

test-plugin.so: test-plugin.cc libir.h $(LIBIR_SO) $(LIBIR_GCC_PLUGIN_SO)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -shared test-plugin.cc -o $@ $(LIBS) -lir -lir-gcc-plugin -L.

clean:
	rm *.so *.o

run-test-plugin: test-plugin.so
	LD_LIBRARY_PATH=. gcc -c -fplugin=./test-plugin.so test.c
