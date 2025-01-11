#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "snakeLogic.h"

// Screen dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int shaderProgram;
unsigned int textureAtlas;
unsigned int dirtBlockVAO;
unsigned int wormBodyVAO;
unsigned int appleVAO;
unsigned int wormHeadVAO;
unsigned int dirtWithGrassVAO;

GLFWwindow* window;

SnakeLogic snakeLogic;

const double MOVE_INTERVAL = 0.4f; // seconds
double lastMoveTime = 0.f; // seconds

const double DEBUG_TOGGLE_INTERVAL = 0.4f; // seconds
double lastDebugToggleTime = 0.f; // seconds
bool isWireFrameModeOn = false;

// apple rotation
const double APPLE_ROTATION_INTERVAL = 0.01; // seconds
double lastAppleRotationTime = 0.0f; // seconds
float appleRotationAngel = 0; // degrees

// Rotation angels for cube (degrees)
float xAngel = 0; // rotation about x axis (up/down)
float yAngel = 0; // rotation about y axis (left/right)

void loadTextureAtlas()
{
    glGenTextures(1, &textureAtlas);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true); 

    int width, height, nrChannels;
    unsigned char *data = stbi_load("Snake3DTextureAtlas.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture atlas!\n";
    }
    stbi_image_free(data);
}

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

void makeDirtWithGrassVAO()
{
    const float position[] = 
    {
        // Front face
        // A, C, B
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // D, B, C
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Back face
        // E, F, G
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // H, G, F
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        // Top face
        // E, A, F
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // B, F, A
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        // Left face
        // A, E, C
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // G, C, E
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        // Right face
        // B, D, F
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // H, F, D
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,

        // Bottom face
        // G, H, C
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // D, C, H
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f
    };

    const float texCoords[] = 
    {
        // Front face
        // A, C, B
        0.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        // D, B, C
        1.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 3.0f/6.0f,

        // Back face
        // E, F, G
        1.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 3.0f/6.0f,
        // H, G, F
        0.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 3.0f/6.0f,
        0.0f/4.0f, 4.0f/6.0f,

        // Top face
        // E, A, F
        1.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 3.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        // B, F, A
        2.0f/4.0f, 3.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 3.0f/6.0f,

        // Left face
        // A, E, C
        1.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 3.0f/6.0f,
        // G, C, E
        0.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 3.0f/6.0f,
        0.0f/4.0f, 4.0f/6.0f,

        // Right face
        // B, D, F
        0.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        // H, F, D
        1.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 3.0f/6.0f,

        // Bottom face
        // G, H, C
        2.0f/4.0f, 4.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,
        // D, C, H
        3.0f/4.0f, 3.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
    };

    unsigned int VBO_position, VBO_texCoords;

    glGenVertexArrays(1, &dirtWithGrassVAO);
    glBindVertexArray(dirtWithGrassVAO);

    glGenBuffers(1, &VBO_position);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 & sizeof(float), 0);
    glEnableVertexAttribArray(1);
}

void makeAppleVAO()
{
    const float position[] = 
    {
        // Front face
        // A, C, B
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // D, B, C
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Back face
        // E, F, G
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // H, G, F
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        // Top face
        // E, A, F
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // B, F, A
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        // Left face
        // A, E, C
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // G, C, E
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        // Right face
        // B, D, F
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // H, F, D
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,

        // Bottom face
        // G, H, C
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // D, C, H
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f
    };

    const float texCoords[] = 
    {
        // Front face
        // A, C, B
        3.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
        // D, B, C
        4.0f/4.0f, 4.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,

        // Back face
        // E, F, G
        2.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 5.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        // H, G, F
        1.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 5.0f/6.0f,

        // Top face
        // E, A, F
        1.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        // B, F, A
        2.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,

        // Left face
        // A, E, C
        3.0f/4.0f, 5.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        // G, C, E
        2.0f/4.0f, 4.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,

        // Right face
        // B, D, F
        4.0f/4.0f, 4.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        // H, F, D
        3.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,

        // Bottom face
        // G, H, C
        1.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 5.0f/6.0f,
        // D, C, H
        2.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 5.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
    };

    unsigned int VBO_position, VBO_texCoords;

    glGenVertexArrays(1, &appleVAO);
    glBindVertexArray(appleVAO);

    glGenBuffers(1, &VBO_position);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 & sizeof(float), 0);
    glEnableVertexAttribArray(1);
}

void makeWormBodyVAO()
{
    const float position[] = 
    {
        // Back face
        // E, F, G
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // H, G, F
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        // Top face
        // E, A, F
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // B, F, A
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        // Left face
        // A, E, C
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // G, C, E
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        // Right face
        // B, D, F
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // H, F, D
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,

        // Bottom face
        // G, H, C
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // D, C, H
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f
    };

    const float texCoords[] = 
    {
        // Back face
        // E, F, G
        1.0f/4.0f, 5.0f/6.0f,
        0.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        // H, G, F
        0.0f/4.0f, 4.0f/6.0f,
        1.0f/4.0f, 4.0f/6.0f,
        0.0f/4.0f, 5.0f/6.0f,

        // Top face
        // E, A, F
        4.0f/4.0f, 5.0f/6.0f,
        4.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 5.0f/6.0f,
        // B, F, A
        3.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 5.0f/6.0f,
        4.0f/4.0f, 6.0f/6.0f,

        // Left face
        // A, E, C
        3.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 5.0f/6.0f,
        4.0f/4.0f, 6.0f/6.0f,
        // G, C, E
        4.0f/4.0f, 5.0f/6.0f,
        4.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 5.0f/6.0f,

        // Right face
        // B, D, F
        4.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 6.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
        // H, F, D
        3.0f/4.0f, 5.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 6.0f/6.0f,

        // Bottom face
        // G, H, C
        3.0f/4.0f, 5.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 6.0f/6.0f,
        // D, C, H
        4.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 6.0f/6.0f,
        4.0f/4.0f, 5.0f/6.0f,
    };

    unsigned int VBO_position, VBO_texCoords;

    glGenVertexArrays(1, &wormBodyVAO);
    glBindVertexArray(wormBodyVAO);

    glGenBuffers(1, &VBO_position);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 & sizeof(float), 0);
    glEnableVertexAttribArray(1);
}

void makeWormHeadVAO()
{
    const float position[] = 
    {
        // Front face
        // A, C, B
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // D, B, C
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Back face
        // E, F, G
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // H, G, F
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        // Top face
        // E, A, F
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // B, F, A
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        // Left face
        // A, E, C
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // G, C, E
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        // Right face
        // B, D, F
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // H, F, D
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,

        // Bottom face
        // G, H, C
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // D, C, H
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f
    };

    const float texCoords[] = 
    {
        // Front face
        // A, C, B
        0.0f/4.0f, 6.0f/6.0f,
        0.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 6.0f/6.0f,
        // D, B, C
        1.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 6.0f/6.0f,
        0.0f/4.0f, 5.0f/6.0f,

        // Back face
        // E, F, G
        1.0f/4.0f, 3.0f/6.0f,
        0.0f/4.0f, 3.0f/6.0f,
        1.0f/4.0f, 2.0f/6.0f,
        // H, G, F
        0.0f/4.0f, 2.0f/6.0f,
        1.0f/4.0f, 2.0f/6.0f,
        0.0f/4.0f, 3.0f/6.0f,

        // Top face
        // E, A, F
        3.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 6.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        // B, F, A
        2.0f/4.0f, 6.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 6.0f/6.0f,

        // Left face
        // A, E, C
        2.0f/4.0f, 6.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 6.0f/6.0f,
        // G, C, E
        1.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 6.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,

        // Right face
        // B, D, F
        2.0f/4.0f, 6.0f/6.0f,
        1.0f/4.0f, 6.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        // H, F, D
        1.0f/4.0f, 5.0f/6.0f,
        2.0f/4.0f, 5.0f/6.0f,
        1.0f/4.0f, 6.0f/6.0f,

        // Bottom face
        // G, H, C
        2.0f/4.0f, 5.0f/6.0f,
        3.0f/4.0f, 5.0f/6.0f,
        2.0f/4.0f, 6.0f/6.0f,
        // D, C, H
        3.0f/4.0f, 6.0f/6.0f,
        2.0f/4.0f, 6.0f/6.0f,
        3.0f/4.0f, 5.0f/6.0f,
    };

    unsigned int VBO_position, VBO_texCoords;

    glGenVertexArrays(1, &wormHeadVAO);
    glBindVertexArray(wormHeadVAO);

    glGenBuffers(1, &VBO_position);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 & sizeof(float), 0);
    glEnableVertexAttribArray(1);
}

void makeDirtBlockVAOAndVBO()
{
    const float position[] = 
    {
        // Front face
        // A, C, B
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // D, B, C
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Back face
        // E, F, G
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // H, G, F
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        // Top face
        // E, A, F
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // B, F, A
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        // Left face
        // A, E, C
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // G, C, E
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        // Right face
        // B, D, F
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        // H, F, D
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,

        // Bottom face
        // G, H, C
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        // D, C, H
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f
    };

    const float texCoords[] = 
    {
        // Front face
        // A, C, B
        2.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        // D, B, C
        3.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,

        // Back face
        // E, F, G
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        3.0f/4.0f, 3.0f/6.0f,
        // H, G, F
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 3.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,

        // Top face
        // E, A, F
        2.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        // B, F, A
        3.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,

        // Left face
        // A, E, C
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        3.0f/4.0f, 3.0f/6.0f,
        // G, C, E
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 3.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,

        // Right face
        // B, D, F
        2.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        // H, F, D
        3.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 3.0f/6.0f,

        // Bottom face
        // G, H, C
        2.0f/4.0f, 3.0f/6.0f,
        3.0f/4.0f, 3.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        // D, C, H
        3.0f/4.0f, 4.0f/6.0f,
        2.0f/4.0f, 4.0f/6.0f,
        3.0f/4.0f, 3.0f/6.0f,
    };

    unsigned int VBO_position, VBO_texCoords;

    glGenVertexArrays(1, &dirtBlockVAO);
    glBindVertexArray(dirtBlockVAO);

    glGenBuffers(1, &VBO_position);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 & sizeof(float), 0);
    glEnableVertexAttribArray(1);
}

void makeShaderProgram()
{
    // Create shader program

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   TexCoord = aTexCoord;\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D ourTexture;"
        "void main()\n"
        "{\n"
        "   FragColor = texture(ourTexture, TexCoord);\n"
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
    glEnable(GL_DEPTH_TEST);
}

void makeCubeVAOAndVBO()
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
}

void drawDirtCube(const glm::mat4& parent)
{   
    const unsigned int SIZE_OF_CUBE = 5;
    const int STARTING_INDEX = SIZE_OF_CUBE - ceil(((double) SIZE_OF_CUBE)/2.0); // positive
    const int LAST_INDEX = STARTING_INDEX - (SIZE_OF_CUBE - 1); // negative

    for (int y  = STARTING_INDEX; y >= LAST_INDEX - 15; y--)
    {
        if (y == STARTING_INDEX)
        {
            glBindVertexArray(dirtWithGrassVAO);
        }
        else
        {
            glBindVertexArray(dirtBlockVAO);
        }

        for (int x = STARTING_INDEX + 15; x >= LAST_INDEX - 15; x--)
        {
            for (int z = STARTING_INDEX; z >= LAST_INDEX - 15; z--)
            {
                // skip zone where worm moves
                if (
                    y <= STARTING_INDEX && y >= LAST_INDEX &&
                    x <= STARTING_INDEX && x >= LAST_INDEX &&
                    z <= STARTING_INDEX && z >= LAST_INDEX
                )
                {
                    continue;
                }

                glm::mat4 child = glm::mat4(1.0f);
                child = glm::translate(child, glm::vec3(x,y,z));
                glm::mat4 model = glm::mat4(parent * child);

                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
}

void setSnakePartModel(int x, int y, int z, Direction dir, const glm::mat4& parent)
{
    glm::mat4 child = glm::mat4(1.0f);
    child = glm::translate(child, glm::vec3(x,y,z));

    if (dir == Direction::Left)
    {
        child = glm::rotate(child, glm::radians(-90.f), glm::vec3(0,1,0));
    }
    else if (dir == Direction::Right)
    {
        child = glm::rotate(child, glm::radians(90.f), glm::vec3(0,1,0));
    }
    else if (dir == Direction::Up)
    {
        child = glm::rotate(child, glm::radians(-90.f), glm::vec3(1,0,0));
    }
    else if (dir == Direction::Down)
    {
        child = glm::rotate(child, glm::radians(90.f), glm::vec3(1,0,0));
    }
    else if (dir == Direction::Backward)
    {
        child = glm::rotate(child, glm::radians(180.f), glm::vec3(0,1,0));
    }
    // else dir == Forward
        // no rotation

    glm::mat4 model = glm::mat4(parent * child);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

void drawApple(int x, int y, int z, const glm::mat4& parent)
{
    glBindVertexArray(appleVAO);

    glm::mat4 child = glm::mat4(1.0f);
    child = glm::translate(child, glm::vec3(x,y,z));
    child = glm::scale(child, glm::vec3(.70f, .70f, .70f));
    child = glm::rotate(child, glm::radians(appleRotationAngel), glm::vec3(0,1,0));
    glm::mat4 model = glm::mat4(parent * child);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36); 
}

void render()
{
    // clear screen
    glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    drawDirtCube(parent);

    // Draw snake

    // snake head
    const SnakePart head = snakeLogic.getSnake()[0];
    setSnakePartModel(head.x, head.y, head.z, head.dir, parent);
    glBindVertexArray(wormHeadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // snake body
    for (int i = 1; i < snakeLogic.getSnakeSize(); i++)
    {
        SnakePart part = snakeLogic.getSnake()[i];
        setSnakePartModel(part.x, part.y, part.z, part.dir, parent);
        glBindVertexArray(wormBodyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 30); 
    }

    // Draw apples
    for (int i = 0; i < snakeLogic.getApplesSize(); i++)
    {
        Apple apple = snakeLogic.getApples()[i];
        drawApple(apple.x, apple.y, apple.z, parent);
    }
}

void processInput(GLFWwindow *window)
{   
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (glfwGetTime() - lastDebugToggleTime >= DEBUG_TOGGLE_INTERVAL)
        {
            if (isWireFrameModeOn) // fill
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            }
            else // wireframe
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            }

            // toggle
            isWireFrameModeOn = !isWireFrameModeOn;
            lastDebugToggleTime = glfwGetTime();
        }
    }

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

    // Check if snake can't move
    if (glfwGetTime() - lastMoveTime < MOVE_INTERVAL)
    {
        return; // can't move, so skip movement input
    }

    // up (w)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Up);
        lastMoveTime = glfwGetTime();
        return;
    }
    // down (s)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Down);
        lastMoveTime = glfwGetTime();
        return;
    }
    // left (a)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Left);
        lastMoveTime = glfwGetTime();
        return;
    }
    // right (d)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Right);
        lastMoveTime = glfwGetTime();
        return;
    }
    // forward (q)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Forward);
        lastMoveTime = glfwGetTime();
        return;
    }
    // backward (e)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        snakeLogic.move(Direction::Backward);
        lastMoveTime = glfwGetTime();
        return;
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
    // makeCubeVAOAndVBO();
    makeDirtBlockVAOAndVBO();
    makeWormBodyVAO();
    makeAppleVAO();
    makeDirtWithGrassVAO();
    makeWormHeadVAO();
    makeShaderProgram();

    // load texture atlas 
    loadTextureAtlas();

    double time_of_last_log = 0;
    double log_interval = 1.0f; // 1 second

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  // Cull back faces (we want to see only the front)
    glFrontFace(GL_CCW);   // Define front-facing triangles to be counter-clockwise

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        if (snakeLogic.isDead())
        {
            snakeLogic.reset();
        }

        if (glfwGetTime() - lastAppleRotationTime >= APPLE_ROTATION_INTERVAL)
        {
            appleRotationAngel += 1;
            lastAppleRotationTime = glfwGetTime();

            if (appleRotationAngel > 360.0f)
            {
                appleRotationAngel = 0.0f;
            }
        }

        processInput(window);
        
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // close window
    glfwTerminate();

    return 0;
}