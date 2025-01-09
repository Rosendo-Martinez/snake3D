#include "snakeLogic.h"
#include <cmath>
#include <cstdlib>
// #include <time.h>  

SnakePart::SnakePart() : SnakePart(0,0,0)
{
}

SnakePart::SnakePart(int x, int y, int z) : x(x), y(y), z(z)
{
//   /* initialize random seed: */
//   srand (time(NULL));
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

    // handle apple generation logic
    generateApple();
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


// generateApple()
    // apple generation is based on if  player has made enough moves
    // if they made enough moves, and not max number of apples, generate apple
void SnakeLogic::generateApple()
{
    // Should be called within move();

    // Can generate apple if player has moved enough times, and not max apples generated.
    if (moveCountSinceLastAppleGen >= appleGenRate && applesSize < MAX_APPLES) // can generate apple
    {
        moveCountSinceLastAppleGen = 0;

        const int STARTING_INDEX = cubeSize - ceil(((double) cubeSize)/2.0);

        // generate apple at random sub cube
        Apple apple;
        apple.x = STARTING_INDEX - (rand() % cubeSize); 
        apple.y = STARTING_INDEX - (rand() % cubeSize); 
        apple.z = STARTING_INDEX - (rand() % cubeSize);

        apples[applesSize] = apple;
        applesSize++;

        return;
    }
    else // can't generate apple
    {
        // Assumes player moved / was called from move()
        moveCountSinceLastAppleGen++;
        return;
    }
}

Apple::Apple() : x(0), y(0), z(0)
{}

Apple::Apple(int x, int y, int z) : x(x), y(y), z(z)
{}

/**
 * The apples that are currently spawned.
 */
const Apple * SnakeLogic::getApples()
{
    return apples;
}

/**
 * The number of apples currently spawned.
 */
const int SnakeLogic::getApplesSize()
{
    return applesSize;
}