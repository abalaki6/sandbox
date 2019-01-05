INC=$$(pkg-config --libs glfw3) -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lboost_program_options

all: heat3d

heat3d: shader.o glad.o heat_parameters.o heat3d_main.cpp
	g++ heat3d_main.cpp glad.o shader.o  -o heat3d $(INC)

shader.o: shader.cpp
	g++ shader.cpp glad.c -c $(INC)

heat_parameters.o : heat_parameters.cpp
	g++ heat_parameters.cpp -c $(INC)

clean:
	rm -f shader.o glad.o heat_parameters.o simple.o heat3d