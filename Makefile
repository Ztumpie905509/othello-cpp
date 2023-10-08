CPPFLAGS = -std=c++17 -Ofast
SRCS = main.cpp Engine/GameEngine.cpp Engine/Board.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = main

.PHONY: all
all: $(EXE) 

debug: CPPFLAGS += -fsanitize=leak,address,undefined -g -DDEBUG
debug: $(OBJS)
	g++ $(CPPFLAGS) -o $(EXE) $(OBJS)
	rm -vf $(OBJS)
	echo "w\n15" | ./$(EXE)

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