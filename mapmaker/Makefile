# Template MakeFile for RayLib projects
CC = g++
CFLAGS = -O0 -Wall  -Wno-missing-braces -g

RAYLIB_PATH = C:/raylib/raylib/src  # Use this if you don't want to copy libraylib.a and raylib.h for every project
LIB = -L.  # -L$(RAYLIB_PATH)
INCLUDE = -I.  # -I$(RAYLIB_PATH)
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm #-mwindows  // For no console

SRC = *.c++# Directory to all source files
EXCLUDE = mapmaker.c++

OUT = main  # Name of the output exe file

build:
	$(CC)  $(filter-out $(EXCLUDE),$(SRC)) -o $(OUT) $(CFLAGS) $(INCLUDE) $(LIB) $(LDFLAGS)
make_map:
	$(CC) mapmaker.c++ -o $(OUT)  $(CFLAGS) $(INCLUDE) $(LIB) $(LDFLAGS)
