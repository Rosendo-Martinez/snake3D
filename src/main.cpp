#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

#include "snakeLogic.h"

// Screen dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int shaderProgram;

GLFWwindow* window;

SnakeLogic snakeLogic;

// Rotation angels for cube (degrees)
float xAngel = 0; // rotation about x axis (up/down)
float yAngel = 0; // rotation about y axis (left/right)

bool init()
{
    // initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window 
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // load opengl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // initial viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    return true;
}

void setupToDrawCube()
{
    // Square Data 

    const float vertices[] = 
    {
        // front vertices
        -0.5f,  0.5f, 0.5f, // top left
         0.5f,  0.5f, 0.5f, // top right
         0.5f, -0.5f, 0.5f, // bottom right
        -0.5f, -0.5f, 0.5f, // bottom left

        // back vertices
        -0.5f,  0.5f, -0.5f, // top left
         0.5f,  0.5f, -0.5f, // top right
         0.5f, -0.5f, -0.5f, // bottom right
        -0.5f, -0.5f, -0.5f, // bottom left
    };

    unsigned int indices[] = 
    {
        // front face
        3, 1, 0,
        3, 2, 1,
        // back face
        7, 4, 5,
        7, 5, 6,
        // top face
        0, 5, 4,
        0, 1, 5,
        // bottom face
        3, 7, 6,
        3, 6, 2,
        // left face
        3, 0, 4,
        3, 4, 7,
        // right face
        2, 5, 1,
        2, 6, 5
    };

    // Create buffers, and attributes

    unsigned int VAO, VBO, EBO;

    // Generate ids
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Upload vertex data to opengl
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Upload indices data to opengl
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Create shader program

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
    
    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    // Create vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // If compilation failed, log it
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // If compilation failed, log it
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // If linking failed, log it
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Use shader
    glUseProgram(shaderProgram);
}

void drawCube()
{
    // Clear screen
    glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // wire frame mode (temp)
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    
    glm::mat4 parent = glm::mat4(1.0f);
    parent = glm::rotate(parent, glm::radians(yAngel), glm::vec3(0.0, 1.0, 0.0));
    parent = glm::rotate(parent, glm::radians(xAngel), glm::vec3(1.0, 0.0, 0.0));

    const unsigned int SIZE_OF_CUBE = 3;
    const int STARTING_INDEX = SIZE_OF_CUBE - ceil(((double) SIZE_OF_CUBE)/2.0);
    const int LAST_INDEX = STARTING_INDEX - (SIZE_OF_CUBE - 1);

    // Render each sub-cube
    for (int z = STARTING_INDEX; z >= LAST_INDEX; z--)
    {
        for (int y = STARTING_INDEX; y >= LAST_INDEX; y--)
        {
            for (int x = STARTING_INDEX; x >= LAST_INDEX; x--)
            {   
                glm::mat4 child = glm::mat4(1.0f);
                child = glm::translate(child, glm::vec3(x,y,z));
                glm::mat4 model = glm::mat4(parent * child);

                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
    }
}

void drawSubCube(int x, int y, int z)
{
    // BAD ASSUMPTION but: assumes was called AFTER call to drawCube

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glm::mat4 parent = glm::mat4(1.0f);
    parent = glm::rotate(parent, glm::radians(yAngel), glm::vec3(0.0, 1.0, 0.0));
    parent = glm::rotate(parent, glm::radians(xAngel), glm::vec3(1.0, 0.0, 0.0));

    glm::mat4 child = glm::mat4(1.0f);
    child = glm::translate(child, glm::vec3(x,y,z));
    child = glm::scale(child, glm::vec3(.8f, .8f, .8f));
    glm::mat4 model = glm::mat4(parent * child);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void processInput(GLFWwindow *window)
{   
    // Cube movement

    // up
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        xAngel += 1;
    }
    // down
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        xAngel -= 1;
    }
    // left
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        yAngel -= 1;
    }
    // right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        yAngel += 1;
    }

    // Snake movement

    // up (w)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Up);
    }
    // down (s)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Down);
    }
    // left (a)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Left);
    }
    // right (d)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Right);
    }
    // forward (q)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Forward);
    }
    // backward (e)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Backward);
    }
}

int main()
{
    std::cout << "Hello, 3D snake!\n";

    // Initialize: glfw, window, and glad
    if (!init())
    {
        return -1;
    }

    // Note: binds VAO, and uses shader program
    // No need to rebind VAO or shader program
    setupToDrawCube();

    double time_of_last_log = 0;
    double log_interval = 1.0f; // 1 second

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetTime() - time_of_last_log >= log_interval)
        {
            std::cout << log_interval << " second passed!\n";
            time_of_last_log = glfwGetTime();
        }

        processInput(window);
        drawCube();

        // Draw snake
        for (int i = 0; i < snakeLogic.getSnakeSize(); i++)
        {
            SnakePart part = snakeLogic.getSnake()[i];
            drawSubCube(part.x, part.y, part.z);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // close window
    glfwTerminate();

    return 0;
}