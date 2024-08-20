NAME = ircserv
CC = c++
CFLAGS = -g -Wall -Wextra -Werror -std=c++98

SRCS = ./srcs/main.cpp \
		./srcs/Server.cpp \
		./srcs/ServerCommands.cpp \
		./srcs/Channel.cpp

OBJS = ${SRCS:.cpp=.o}

all: ${NAME}

${NAME}: ${OBJS}
		@${CC} ${CFLAGS} -o ${NAME} ${OBJS}

%.o: %.cpp ./srcs/Server.hpp ./srcs/Client.hpp ./srcs/Channel.hpp
		${CC} ${CFLAGS} -c $< -o $@

clean:
		@rm -f ${OBJS}

fclean: clean
		@rm -f ${NAME} a.out

re: fclean all

.PHONY: all clean fclean re