##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## Makefile
##

SRC	=	main.cpp

OBJ = 	$(SRC:.cpp=.o)

NAME = 	babel

LDLIBS	 = -lpthread -lportaudio -lopus

CXXFLAGS = -std=c++17 -Wall -Wextra -Weffc++

CPPFLAGS = -iquote include -iquote Audio -iquote Serializer

CXX = g++

all :	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDLIBS)

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re: fclean all