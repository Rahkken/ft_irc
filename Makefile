CC = c++ -g -Wall -Wextra -Werror -std=c++98

SRCS = ./srcs/main.cpp \
		./srcs/Server.cpp \
		./srcs/Channel.cpp

NAME = ircserv

${NAME} :
	${CC} ${SRCS} -o ${NAME}

all : ${NAME}

fclean :
	rm -rf ${NAME}

re : fclean all