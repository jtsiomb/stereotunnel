src = $(wildcard src/*.c) $(wildcard src/glut/*.c)
ccsrc = $(wildcard src/*.cc)

obj = $(src:.c=.o) $(ccsrc:.cc=.o)
dep = $(obj:.o=.d)
bin = test

incdir = -Isrc
def = -DNO_FREETYPE

CFLAGS = -pedantic -Wall -g $(def) $(incdir)
CXXFLAGS = $(CFLAGS)
LDFLAGS = $(libgl) -lm -ldl

include libs/Makefile

sys := $(shell uname -s)
ifeq ($(sys), Darwin)
	libgl = -framework OpenGL -framework GLUT -lGLEW
else
	libgl = -lGL -lGLU -lglut -lGLEW
endif

$(bin): $(obj)
	@echo "Linking $(bin) ..."
	@$(CXX) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

%.d: %.c
	@$(CPP) $(CFLAGS) -MM -MT $(@:.d=.o) $< >$@

%.d: %.cc
	@$(CPP) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< >$@

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: cleandep
cleandep:
	rm -f $(dep)
