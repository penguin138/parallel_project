INCLUDES1 = -I "./headers"
INCLUDES2 = -I "./src/shell"

all:
		clang++ -g  -o life ${INCLUDES1}  ${INCLUDES2} $(wildcard ./src/*.cpp) $(wildcard ./src/shell/*.cpp)
