all: range_example recursive_dir_example

range_example: range_example.cc generators.h
	g++ -o range_example range_example.cc -std=c++11 -pthread

recursive_dir_example: recursive_dir_example.cc generators.h
	g++ -o recursive_dir_example recursive_dir_example.cc -std=c++11 -pthread