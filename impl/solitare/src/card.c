/*
 * Copyright 2024 Cheban Sergiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include "../inc/solitare.h"

/*
 * Arrays of card display elements
 * Used for converting card values to display text
 */
char *numerals[] = {
    " A", " 2", " 3", " 4", " 5", " 6",
    " 7", " 8", " 9", "10", " J", " Q", " K"
};

wchar_t suits[] = {
    L'♠', L'♥', L'♣', L'♦'
};

/*
 * Border characters for screen elements
 * Used for drawing borders around cards and game areas
 */
wchar_t fat_border[8] = {
    L'═', L'║', L'╔', L'╗', L'╚', L'╝', L'╠', L'╣'
};

wchar_t card_border[8] = {
    L'─', L'│', L'┌', L'┐', L'└', L'┘', L'├', L'┤'
};

/*
 * Convert suit enum to display character
 * Returns Unicode character for card suit
 */
wchar_t suit_to_text(Suit suit) {
    return suits[suit];
}

/*
 * Convert numeral enum to display text
 * Returns string representation of card value
 */
char *numeral_to_text(Numeral numeral) {
    return numerals[numeral - 1];
}

/*
 * Draw card on screen at specified position
 * Handles card borders, suit symbols, and numerals
 */
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x) {
    if (!card) return;

    int x_0 = x;
    int y_0 = y;

    fill_area(screen, y_0, x_0, size_y, size_x, ' ');
    colorize_card(screen, card, y_0 + 1, x_0 + 1, CARD_HEIGHT - 2, CARD_WIDTH - 2);
    add_borders(screen, y_0, x_0, size_y, size_x, card_border);

    if (y != CARD_HEIGHT + 2 * BORDER_OFFSET_Y + 1 && y != BORDER_OFFSET_Y) {
        screen->data[y_0][x_0] = card_border[6];
        screen->data[y_0][x_0 + CARD_WIDTH - 1] = card_border[7];
    }

    if (card->selected) {
        screen->data[y_0][x_0 + 3] = L'◖';
        screen->data[y_0][x_0 + 4] = L'◗';
    }

    wchar_t suit = suit_to_text(card->suit);
    screen->data[y_0 + 1][x_0 + 1] = suit;
    screen->data[y_0 + CARD_HEIGHT / 2][x_0 + CARD_WIDTH / 2 - 1] = suit;
    screen->data[y_0 + CARD_HEIGHT - 2][x_0 + CARD_WIDTH - 3] = suit;

    char *numeral = numeral_to_text(card->numeral);
    screen->data[y_0 + 1][x_0 + CARD_WIDTH - 3] = numeral[0];
    screen->data[y_0 + 1][x_0 + CARD_WIDTH - 2] = numeral[1];
    if (numeral[0] == ' ') {
        screen->data[y_0 + CARD_HEIGHT - 2][x_0 + 1] = numeral[1];
    }
    else {
        screen->data[y_0 + CARD_HEIGHT - 2][x_0 + 1] = numeral[0];
        screen->data[y_0 + CARD_HEIGHT - 2][x_0 + 2] = numeral[1];
    }
}

/*
 * Apply color formatting to card area
 * Sets background color and text color based on suit
 */
void colorize_card(Screen *screen, const Card *card, int y, int x, int height, int width) {
    if (y < 0 || x < 0 || y + height > SCREEN_HEIGHT || x + width > SCREEN_WIDTH) {
        return;
    }

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            if (i < SCREEN_HEIGHT && j < SCREEN_WIDTH) {
                screen->background[i][j] = COLOR_WHITE;
                screen->data[i][j] = ' ';
                screen->foreground[i][j] = COLOR_RESET;
            }
        }
    }

    screen->foreground[y][x] = (card->suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;
    screen->foreground[y + CARD_HEIGHT / 2 - 1][x + CARD_WIDTH / 2 - 1 - 1] = (card->suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;
    screen->foreground[y + CARD_HEIGHT - 2 - 1][x + CARD_WIDTH - 3 - 1] = (card->suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;

    screen->foreground[y][x + width - 2] = COLOR_BLACK;
    screen->foreground[y][x + width - 1] = COLOR_BLACK;
    screen->foreground[y + height - 1][x] = COLOR_BLACK;
    if (card->numeral % 10 == 0) screen->foreground[y + height - 1][x + 1] = COLOR_BLACK;
}