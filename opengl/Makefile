INC=$$(pkg-config --libs glfw3) -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lboost_program_options

all: simple

simple: shader.o glad.o heat_parameters.o opengl.cpp
	g++ opengl.cpp glad.o shader.o -o simple $(INC)

shader.o: shader.cpp
	g++ shader.cpp glad.c -c $(INC)

heat_parameters.o : heat_parameters.cpp
	g++ heat_parameters.cpp -c $(INC)

clean:
	rm -f shader.o glad.o heat_parameters.o simple.o