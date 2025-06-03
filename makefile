main: main.o
	g++ -o main main.o -lSDL3 -lGL -lGLEW

main.o: main.cpp
	g++ -c -o main.o main.cpp

clean:
	rm *.o
