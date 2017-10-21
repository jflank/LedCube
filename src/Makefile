CC=g++

IDIR=../include
IFLAG=-I$(IDIR)

ODIR=obj
#Make sure object directory exists
dummyODIR := $(shell test -d $(ODIR) || mkdir -p $(ODIR))

CPPFLAGS=-g -std=c++11 -ggdb 
LDFLAGS=-g 
RM=rm -f

_DEPS=Ledcube.h GLcube.h Solve5x5.h Solve8.h Serialcube.h AnimCube.h LedcubeFactory.h Dijkstracube.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

SRCS=Dijkstracube.cpp main.cpp Ledcube.cpp GLcube.cpp Solve5x5.cpp easylogging++.cpp Solve8.cpp Serialcube.cpp AnimCube.cpp Images.cpp LedcubeFactory.cpp 

_OBJS = $(subst .cpp,.o,$(SRCS)) 
OBJS=$(patsubst %,$(ODIR)/%,$(_OBJS))

EXE=LedCube

LIB=-L/usr/X11R6/lib/ -lGL -lGLU -lglut -lX11 -l pthread

$(phony all): $(EXE) 

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIB) 

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -c -o $@ $< $(IFLAG)

run: $(EXE)
	./$(EXE) -g -s

.PHONY: clean

clean: 
	$(RM) $(OBJS)
	$(RM) $(EXE)
	$(RM) *~
	$(RM) $(IDIR)/*~
	$(RM) core


