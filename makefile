OPTS = -Wall -Wextra -iquote headers/

main: main.o shader.o mesh.o meshUtils.o
	g++ $(OPTS) -o main main.o shader.o mesh.o meshUtils.o -lSDL3 -lGL -lGLEW

main.o: main.cpp headers/math.hpp headers/shader.hpp headers/mesh.hpp headers/meshUtils.hpp
	g++ $(OPTS) -c -o main.o main.cpp

shader.o: shader.cpp headers/shader.hpp headers/math.hpp
	g++ $(OPTS) -c -o shader.o shader.cpp

mesh.o: mesh.cpp headers/mesh.hpp headers/math.hpp
	g++ $(OPTS) -c -o mesh.o mesh.cpp

meshUtils.o: meshUtils.cpp headers/meshUtils.hpp headers/mesh.hpp headers/math.hpp
	g++ $(OPTS) -c -o meshUtils.o meshUtils.cpp

headers/shader.hpp: headers/math.hpp
headers/mesh.hpp: headers/math.hpp
headers/meshUtils.hpp: headers/mesh.hpp

clean:
	rm *.o
