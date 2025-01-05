
main : ./src/main.cpp ./src/snakeLogic.cpp ./src/snakeLogic.h
	g++ ./src/main.cpp ./dep/glad/src/glad.c ./src/snakeLogic.cpp -o ./bin/main.exe -I./dep/glad/include -I./dep/ -ldl -lglfw

clean :
	