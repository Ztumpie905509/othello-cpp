CPPFLAGS = -std=c++17 -Ofast
SRCS = main.cpp Engine/GameEngine.cpp 
OBJS = $(SRCS:.cpp=.o)
EXE = main

.PHONY: all
all: $(EXE) 

debug: CPPFLAGS += -fsanitize=leak -g -DDEBUG
debug: $(OBJS)
	g++ $(CPPFLAGS) -o $(EXE) $(OBJS)
	echo "w\n5" | ./$(EXE)

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