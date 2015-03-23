CXX = clang++
CXXFLAGS = -Wall -Wextra

OBJECTS = ${shell find -type f -name *.cpp}

EXEC = emma_watson

${EXEC}: ${OBJECTS}
		${CXX} $^ -o ${EXEC} ${CXXFLAGS}

clean:
		rm -rf emma_watson
