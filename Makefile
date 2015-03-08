SRCDIR = src
INCDIR = include
LIBDIR = lib
CPP = g++
UPX = upx -qq

CFLAGS = -O2 -s -w -I$(INCDIR)/nwbd -I$(INCDIR)/tcod
#CFLAGS = -O0 -g -Wall -I$(INCDIR)/nwbd -I$(INCDIR)/tcod

# Determine if operating system is 32-bit or 64-bit
ifeq ($(shell uname -m),x86_64)

	# These are the definitions specific to 64-bit systems
	LFLAGS = -L$(LIBDIR)/x86_64 -ltcod -ltcodxx -lm -Wl,-rpath=$(LIBDIR)/x86_64

else

	# These are the definitions specific to 32-bit systems
	LFLAGS = -L$(LIBDIR)/i386 -ltcod -ltcodxx -lm -Wl,-rpath=$(LIBDIR)/i386

endif

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CPP) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

HEADERS = $(wildcard $(INCDIR)/nwbd/*.hpp)

OBJECTS = $(SOURCES:.cpp=.o)

all: nwbd bundle-linux

nwbd: $(HEADERS) $(OBJECTS)
	@$(CPP) $(CFLAGS) $(OBJECTS) -o $@ $(LFLAGS)
	@rm -f $(OBJECTS)
#	$(UPX) $@

clean:
	@rm -f $(OBJECTS)
	@rm -f nwbd

bundle-linux:
	@tar -zcf nwbd-linux.tar.gz data lib nwbd
