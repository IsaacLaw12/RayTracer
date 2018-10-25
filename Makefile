SRCDIR := ./src
FILES = $(shell find $(SRCDIR) -name "*.cpp")
ERR_FLAGS = -Wall
C_FLAG = -O3 -I./Eigen/Eigen -std=gnu++11
EXE_NAME = raytracer
all:
	g++ $(ERR_FLAGS) $(C_FLAG) $(FILES) -o $(EXE_NAME)
