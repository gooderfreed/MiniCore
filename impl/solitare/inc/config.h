#ifndef CONFIG_H
#define CONFIG_H

#define MAP_HEIGHT 1
#define MAP_WIDTH 3

#define CARD_NUMERALS 13

#define CARD_SCALE_FACTOR 7
#define CARD_COVERED_HEIGHT 2

#define CARD_HEIGHT CARD_SCALE_FACTOR
#define CARD_WIDTH (CARD_SCALE_FACTOR + 1)

#define BORDER_OFFSET_X 1
#define BORDER_OFFSET_Y 1

#define DECK_OFFSET (CARD_HEIGHT + 2 * BORDER_OFFSET_Y)
#define DECK_SIZE (CARD_SUITS * CARD_NUMERALS)
#define DECK_DEFAULT_COORDS (Coords){.x = 0, .y = 0}

#define FIELD_HEIGHT 19
#define FIELD_WIDTH 7
#define FIELD_DEFAULT_COORDS (Coords){.x = 3, .y = 3}

#define SCREEN_HEIGHT (2 * (FIELD_HEIGHT - 1) + CARD_HEIGHT + DECK_OFFSET + BORDER_OFFSET_X)
#define SCREEN_WIDTH (FIELD_WIDTH * CARD_WIDTH + 2 * BORDER_OFFSET_X)

#define STOCK_X_BASE (SCREEN_WIDTH - BORDER_OFFSET_X - 4 * CARD_WIDTH)
#define STOCK_AREA_HEIGHT (CARD_HEIGHT + 1)
#define STOCK_AREA_WIDTH (4 * CARD_WIDTH)
#define STOCK_CURSOR_BASE_X (STOCK_X_BASE + (CARD_WIDTH / 2 - 1))
#define STOCK_DEFAULT_COORDS (Coords){.x = 1, .y = 0}

#define CONTAINER_SIZE CARD_NUMERALS

#endif