
main : ./src/main.cpp ./bin/snakeLogic.o
	g++ ./src/main.cpp ./dep/glad/src/glad.c ./bin/snakeLogic.o -o ./bin/main.exe -I./dep/glad/include -I./dep/ -ldl -lglfw

./bin/snakeLogic.o : ./src/snakeLogic.h ./src/snakeLogic.cpp
	g++ -c ./src/snakeLogic.cpp -o ./bin/snakeLogic.o

clean :
	