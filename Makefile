SRCDIR := ./src
FILES = $(shell find $(SRCDIR) -name "*.cpp")
ERR_FLAGS = -pedantic -Wall -Wextra
C_FLAG = -O3 -I./Eigen/Eigen -std=gnu++17 -pthread
EXE_NAME = raytracer
all:
	g++ $(ERR_FLAGS) $(C_FLAG) $(FILES) -o $(EXE_NAME)
