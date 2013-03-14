all: range_example file_example

range_example: range_example.cc generators.h
	g++ -o range_example range_example.cc -std=c++11 -pthread

file_example: file_example.cc generators.h
	g++ -o file_example file_example.cc -std=c++11 -pthread