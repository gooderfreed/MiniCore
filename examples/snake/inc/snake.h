#ifndef SNAKE_H
#define SNAKE_H

#define _POSIX_C_SOURCE 199309L

#include "config.h"
#include "zen.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
 * Snake game implementation
 * Extends zen framework with specific snake game logic
 */

/*
 * Menu structure
 * Represents the menu of the game
 */
typedef struct Menu {
    ObjectInterfaces interfaces;     // Zen framework interfaces
    bool start_game : 1;             // Start game flag
} Menu;

/*
 * Controls structure
 * Represents the controls screen of the game
 */
typedef struct Controls {
    ObjectInterfaces interfaces;     // Zen framework interfaces
} Controls;

/*
 * Win screen structure
 * Represents the win screen of the game
 */
typedef struct WinScreen {
    ObjectInterfaces interfaces;    // Zen framework interfaces
} WinScreen;

/*
 * Snake structure
 * Represents the snake entity with movement direction, segments, and state.
 */
typedef struct Snake {
    ObjectInterfaces interfaces; // Zen framework interfaces for interaction
    Coords *segments;            // Array of snake body segments
    Coords direction;            // Current movement direction
    Coords apple;                // Position of the apple
    int length;                  // Current snake length
    bool is_alive;               // Snake's alive status
} Snake;

/*
 * Score counter structure
 * Tracks the current and maximum score in the game.
 */
typedef struct ScoreCounter {
    ObjectInterfaces interfaces; // Zen framework interfaces for interaction
    int cur_score;               // Current game score
    int max_score;               // Highest recorded score
} ScoreCounter;


extern const wchar_t fat_border[8];
extern const wchar_t card_border[8];

//snake
Snake *init_snake(Arena *arena);
MapLayer *game_layer_init(Arena *arena);

//counter
ScoreCounter *init_score_counter(Arena *arena);

#endif
