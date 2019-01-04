INC=$$(pkg-config --libs glfw3) -lGL -lX11 -lpthread -lXrandr -lXi -ldl

all: simple

simple: shader.o glad.o opengl.cpp
	g++ opengl.cpp glad.o shader.o -o simple $(INC)

shader.o: shader.cpp
	g++ shader.cpp glad.c -c $(INC)

clean:
	rm -f shader.o glad.o simple.o