CC=g++
LN=g++
OPT=-std=gnu++17 -Wextra -Werror -fopenmp
LIB=-lm -lrt -lstdc++ -lpthread -fopenmp
OBJ= src/Memory.o src/Context.o src/Token.o src/Expr.o src/Function.o src/main.o

all: OPT += -Os -O3
all: executable
  
debug: OPT += -g -pg -D__DEBUG
debug: executable

executable: $(OBJ)
	$(LN) $(OBJ) -o calc $(LIB)

%.o: %.cpp
	$(CC) $(OPT) -c $< -o $@

clean:
	rm -f $(OBJ) calc
