CC=g++
FLAGS=-lsfml-graphics -lsfml-window -lsfml-system -fopenmp
CFLAGS=-Wall -ggdb -O0

all: ant

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

ant: main.o Ant.o Food.o Wall.o
	$(CC) $^ -o $@ $(FLAGS)


run: ant
	./ant 175

clean:
	rm *.o
	rm ant