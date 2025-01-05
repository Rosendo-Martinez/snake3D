#include "snakeLogic.h"

SnakePart::SnakePart() : SnakePart(0,0,0)
{
}

SnakePart::SnakePart(int x, int y, int z) : x(x), y(y), z(z)
{
}

SnakeLogic::SnakeLogic()
{
}

void SnakeLogic::move(Direction dir)
{
    // Move snake forward (except head)
    // Done by shifting back snake parts in array
    for (int i = snakeSize - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }

    // Update position of head
    if (dir == Direction::Up)
    {
        snake[0].y++;
    }
    else if (dir == Direction::Down)
    {
        snake[0].y--;
    }
    else if (dir == Direction::Left)
    {
        snake[0].x--;
    }
    else if (dir == Direction::Right)
    {
        snake[0].x++;
    }
    else if (dir == Direction::Forward)
    {
        snake[0].z++;
    }
    else // backwards
    {
        snake[0].z--;
    }
}

/**
 * The array of SnakeParts that represent the snake.
 * 
 * The head is at index 0.
 * The tail is at index getSnakeSize() - 1.
 */
const SnakePart * SnakeLogic::getSnake()
{
    return snake;
}

/**
 * The size of the snake.
 */
const int SnakeLogic::getSnakeSize()
{
    return snakeSize;
}