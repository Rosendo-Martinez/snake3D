#define MAX_APPLES 3
#define MAX_SNAKE_SIZE 10

enum class Direction
{
    Up, Down, Right, Left, Forward, Backward
};

struct SnakePart
{
    int x, y, z;
    Direction dir = Direction::Forward;

    SnakePart();
    SnakePart(int x, int y, int z);
};

struct Apple
{
    int x, y, z;

    Apple();
    Apple(int x, int y, int z);
};

class SnakeLogic
{
    SnakePart snake[MAX_SNAKE_SIZE];
    int snakeSize = 0;
    int cubeSize = 3;
    Apple apples[MAX_APPLES];
    int applesSize = 0;
    int moveCountSinceLastAppleGen = 0;
    int appleGenRate = 7;

public:
    SnakeLogic();
    void move(Direction dir);
    const SnakePart * getSnake();
    const int getSnakeSize();
    void reset();
    bool isDead();
    void generateApple();
    const Apple * getApples();
    const int getApplesSize();
    void eatApple();
};