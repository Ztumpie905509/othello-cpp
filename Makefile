CPPFLAGS = -std=c++17 -g -Ofast
SRCS = main.cpp Engine/GameEngine.cpp 
OBJS = $(SRCS:.cpp=.o)
EXE = main.exe

.PHONY: all
all: $(EXE) 

debug: $(OBJS)
	g++ $(CPPFLAGS) -g -o $(EXE) $(OBJS)

linux: $(OBJS)
	g++ $(CPPFLAGS) -fsanitize=leak -g -o $(EXE) $(OBJS)

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
