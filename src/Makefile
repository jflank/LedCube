CPPFLAGS=-g -std=c++11 -Wall -ggdb
LDFLAGS=-g 
RM=rm -f

SRCS=main.cpp Ledcube.cpp GLcube.cpp Solve5x5.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
EXE=LedCube

LIB=-L/usr/X11R6/lib/ -lGL -lGLU -lglut -lX11 -l pthread
INC=

all: $(EXE) Makefile

$(EXE): $(OBJS) 
	g++ $(LDFLAGS) -o $(EXE) $(OBJS) $(LIB) 

main.o: main.cpp Makefile
	g++ $(CPPFLAGS) $(INC) -c main.cpp 

Ledcube.o: Ledcube.cpp Makefile
	g++ $(CPPFLAGS) $(INC) -c Ledcube.cpp 

Sove5x5.o: Solve5x5.cpp Makefile
	g++ $(CPPFLAGS) $(INC) -c Solve5x5.cpp 

GLcube.o: GLcube.cpp Makefile
	g++ $(CPPFLAGS) $(INC) -c GLcube.cpp 

Serial.o: Serial.cpp Makefile
	g++ $(CPPFLAGS) $(INC) -c Serial.cpp 

animations.o: animations.cpp Makefile
	g++ $(CPPFLAGS) $(INC) -c animations.cpp 

run: $(EXE)
	./$(EXE) -g

clean: 
	$(RM) $(OBJS)
	$(RM) $(EXE)
