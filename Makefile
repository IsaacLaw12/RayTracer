SRCDIR = := ./src
FILES = $(shell find $(SRCDIR) -name "*.cpp")
ERR_FLAGS = -Wall
EXE_NAME = raytracer
all:
	g++ $(ERR_FLAGS) -I./Eigen/Eigen $(FILES) -o $(EXE_NAME)
