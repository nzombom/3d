BUILD_DIR = ./build
SRC_DIR = ./src

SRCS = $(shell find $(SRC_DIR) -name *.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

FLAGS = -Wall -Wextra -iquote ./src/headers

LIBS = -lGL -lGLEW -lSDL3

main: $(OBJS)
	g++ $(OBJS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(FLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)/*

-include $(DEPS)
