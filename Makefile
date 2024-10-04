SYSCONF_LINK = g++
CPPFLAGS     = -I./src/headers
LDFLAGS      =
LIBS         = -lm

DESTDIR = ./
TARGET  = main

CPP_SOURCE = $(wildcard ./src/*.cpp)
H_SOURCE   = $(wildcard ./src/headers/*.h)

OBJECTS := $(patsubst %.cpp,%.o,$(CPP_SOURCE))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga