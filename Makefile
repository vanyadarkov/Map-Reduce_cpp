PROJECT=tema1
SOURCES=tema1.cpp utils.cpp thread_utils.cpp

INCPATHS=utils

LIBPATHS=.
LDFLAGS=-Wall -pthread -Werror
CXXFLAGS=-c -Wall -pthread -Werror
CXX=g++

OBJECTS=$(SOURCES:.cpp=.o)
INCFLAGS=$(foreach TMP,$(INCPATHS),-I$(TMP))
LIBFLAGS=$(foreach TMP,$(LIBPATHS),-L$(TMP))
VPATH=src

BINARY=$(PROJECT)
build: all

all: $(SOURCES) $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) $(LIBFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CXX) $(INCFLAGS) $(CXXFLAGS) -fPIC $< -o $@

clean:
	rm -f $(BINARY) $(OBJECTS)
