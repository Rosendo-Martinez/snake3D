
main : ./src/main.cpp 
	g++ ./src/main.cpp ./dep/glad/src/glad.c -o ./bin/main.exe -I./dep/glad/include -I./dep/ -ldl -lglfw

clean :
	