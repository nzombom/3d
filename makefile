OPTS = -Wall -Wextra -iquote headers/

main: main.o shader.o mesh.o headers/math.hpp headers/shader.hpp headers/mesh.hpp
	g++ $(OPTS) -o main main.o shader.o mesh.o -lSDL3 -lGL -lGLEW

main.o: main.cpp headers/math.hpp headers/shader.hpp
	g++ $(OPTS) -c -o main.o main.cpp

shader.o: shader.cpp headers/shader.hpp headers/math.hpp
	g++ $(OPTS) -c -o shader.o shader.cpp

mesh.o: mesh.cpp headers/mesh.hpp headers/math.hpp
	g++ $(OPTS) -c -o mesh.o mesh.cpp

headers/shader.hpp: headers/math.hpp
headers/mesh.hpp: headers/math.hpp

clean:
	rm *.o
