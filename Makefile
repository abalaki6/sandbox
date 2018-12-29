all:
	g++ -g -fopenmp heat_demo.cpp $$(pkg-config --libs --cflags opencv) -o heat_demo