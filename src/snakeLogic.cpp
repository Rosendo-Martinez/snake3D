#include "snakeLogic.h"
#include <cmath>

SnakePart::SnakePart() : SnakePart(0,0,0)
{
}

SnakePart::SnakePart(int x, int y, int z) : x(x), y(y), z(z)
{
}

SnakeLogic::SnakeLogic()
{
    snake[0] = SnakePart(0,0,0); // head
    snake[1] = SnakePart(0,0,-1);
    snake[2] = SnakePart(0,0,-2);

    snakeSize = 3;
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

/**
 * Resets the game state to initial state.
 */
void SnakeLogic::reset()
{
    snake[0] = SnakePart(0,0,0); // head
    snake[1] = SnakePart(0,0,-1);
    snake[2] = SnakePart(0,0,-2);

    snakeSize = 3;   
}

/**
 * Checks if snake is dead.
 */
bool SnakeLogic::isDead()
{
    // if snake ate self
        // is dead
    // if snake is outside cube
        // is dead
    // else
        // is alive

    const SnakePart head = snake[0];

    // Check if snake ate itself
    for (int i = 1; i < snakeSize; i++)
    {
        if (head.x == snake[i].x && head.y == snake[i].y && head.z == snake[i].z)
        {
            return true;
        }
    }

    // Snake is enclosed in cube. It is must have odd length (3x3x3, 5x5x5, etc.).
    // Center sub-cube is centered at origin, and has index (0,0,0,).

    const int STARTING_INDEX = cubeSize - ceil(((double) cubeSize)/2.0);
    const int LAST_INDEX = STARTING_INDEX - (cubeSize - 1);

    // Check if head is outside cube
    if 
    (
        head.x > STARTING_INDEX || head.x < LAST_INDEX ||
        head.y > STARTING_INDEX || head.y < LAST_INDEX ||
        head.z > STARTING_INDEX || head.z < LAST_INDEX 
    )
    {
        return true;
    }

    return false;
}