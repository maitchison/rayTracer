CC=g++

# 'normal' levels of optimizaton
#CC_FLAGS=-std=c++11 -O3

# 'experimental' levels of optimization :)
CC_FLAGS=-std=c++11 -Ofast -floop-nest-optimize -floop-parallelize-all

#LINK_FLAGS=-framework GLUT -framework OpenGL
LINK_FLAGS=-lm -lGL -lGLU -lglut

# File names
EXEC = RayTracer.exe
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
 
# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINK_FLAGS) -o $(EXEC)
 
# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@
 
# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS) 	

run:
	./$(EXEC)
