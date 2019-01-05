CFLAG=-std=c++11 -g
INC=$$(pkg-config --libs glfw3) $$(pkg-config --libs --cflags opencv) -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lboost_program_options

all: heat3d

heat3d: shader.o glad.o heat_parameters.o heat_solver.o camera.o heat3d_main.cpp
	g++ $(CFLAG) heat3d_main.cpp *.o -o heat3d $(INC)

shader.o: shader.cpp
	g++ $(CFLAG) shader.cpp glad.c -c $(INC)

heat_parameters.o: heat_parameters.cpp
	g++ $(CFLAG) heat_parameters.cpp -c $(INC)

heat_solver.o: heat_solver.cpp
	g++ $(CFLAG) heat_solver.cpp -c $(INC)

camera.o: camera.cpp
	g++ $(CFLAG) camera.cpp -c $(INC)

clean:
	rm -f *.o heat3d