all: heat_demo
	
heat_demo: heat_demo.cpp
	g++ -g -fopenmp heat_demo.cpp $$(pkg-config --libs --cflags opencv) -lboost_program_options -o heat_demo

clean:
	rm -f heat_demo *.o