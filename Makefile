CPPFLAGS = -std=c++17 -Ofast -pthread
SRCS = main.cpp Engine/GameEngine.cpp Engine/Board.cpp Engine/Mcts.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = main
DEBUGFLAGS = -g -DDEBUG

ifneq ($(OS),Windows_NT)
    DEBUGFLAGS += -fsanitize=address,undefined,leaks
endif


.PHONY: all
all: $(EXE) 

debug: CPPFLAGS += $(DEBUGFLAGS)
debug: $(OBJS)
	g++ $(CPPFLAGS) -o $(EXE) $(OBJS)
	rm -vf $(OBJS)
	
$(EXE): $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS)
	rm -vf $(OBJS)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

# For removing the objects/executable files
clean:
	rm -vf *.o *.exe $(OBJS) $(EXE)

# makedepend can find the .h dependencies automatically
depend:
	g++ -MM $(SRCS)
	rm -vf *.o
