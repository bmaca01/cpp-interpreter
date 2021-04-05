COMPILER=g++
OPTIONS=-g -std=c++11 -Wall -Wextra -Werror -Wunreachable-code
COMPILE=$(COMPILER) $(OPTIONS)

program: main.cpp io.cpp lex.cpp parse.cpp
	$(COMPILE) main.cpp io.cpp lex.cpp parse.cpp -o program

clean:
	rm -f *.o program
