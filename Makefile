# Compiler
CXX = c++

# Compiler flags
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror 

# Target executable
TARGET = ircserv

HEADER = server.hpp client.hpp channel.hpp command/command.hpp

# Source files
SRCS = main.cpp server.cpp client.cpp channel.cpp command/Ping.cpp command/Join.cpp command/Topic.cpp command/Pass.cpp command/Nick.cpp \
command/User.cpp command/Privmsg.cpp command/Part.cpp command/Kick.cpp command/Invite.cpp command/Mode.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS)

# Clean and remove the target executable
fclean: clean
	rm -f $(TARGET)

re: fclean all

# Default rule
all: $(TARGET)

.PHONY: all clean fclean re