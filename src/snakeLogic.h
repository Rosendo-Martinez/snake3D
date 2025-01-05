struct SnakePart
{
    int x, y, z;

    SnakePart();
    SnakePart(int x, int y, int z);
};

enum class Direction
{
    Up, Down, Right, Left, Forward, Backward
};

class SnakeLogic
{
    SnakePart snake[50];
    int snakeSize = 3;

public:
    SnakeLogic();
    void move(Direction dir);
    const SnakePart * getSnake();
    const int getSnakeSize();
};