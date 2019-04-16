#####################################################################
# Build Configurations

BINARY := trackball
CXX := g++
CC := gcc
CXX_FLAGS := -ggdb -std=c++14 -Wall -Wextra -pipe -DGLM_FORCE_RADIANS
LD_FLAGS := -lGL -ldl -lSDL2 -lSDL2_image -lm
OBJS := main shader sphere
GLOBAL_HEADERS :=

#####################################################################

OBJS_FULL := $(patsubst %,obj/%.o,$(OBJS))
GLOBAL_HEADERS_FULL := $(patsubst %,src/%.hpp,$(GLOBAL_HEADERS))

default: $(OBJS_FULL) obj/glad.o
	$(CXX) -o $(BINARY) obj/glad.o $(OBJS_FULL) $(LD_FLAGS)

$(OBJS_FULL): obj/%.o: src/%.cpp src/%.hpp $(GLOBAL_HEADERS_FULL)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

obj/glad.o: src/glad.c src/glad.h
	$(CC) -c src/glad.c -o obj/glad.o

clean:
	rm -f obj/* $(BINARY)

.PHONY: clean
