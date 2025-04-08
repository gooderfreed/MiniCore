/*
 * Field implementation
 * Handles field object management and card placement
 */
#include "../../inc/solitaire.h"

// =========================
//  FIELD METHODS
// =========================
/*
 * Get last card y position in field
 * Finds the highest y position of a card in the specified column
 */
static int get_last_card_y(const Field *field, int x) {
    for (int i = FIELD_HEIGHT - 1; i >= 0; --i)
        if (field->field[i][x]) return i;
    return 0;
}

/*
 * Clear field
 * Removes all cards from field
 */
static void clear_field(Field *field) {
    for (short row = 0; row < FIELD_WIDTH; row++) {
        for (short col = 0; col < FIELD_HEIGHT; col++) {
            if (field->field[col][row]) {
                field->field[col][row] = NULL;
            }
        }
    }
}

/*
 * Prepare field
 * Initializes field with cards from deck
 */
void field_reset(Field *field, Deck *deck) {
    clear_field(field);
    DeckMethods *deck_methods = (DeckMethods *)(deck->export_methods);
    for (short row = 0; row < FIELD_WIDTH; row++) {
        for (short col = 0; col < FIELD_HEIGHT; col++) {
            if (col > row) field->field[col][row] = NULL;
            else {
                Card *card = deck_methods->draw_card(deck);
                card->coords.x = row;
                card->coords.y = col;
                card->object = Field_enum;
                card->hidden = col == row ? false : true;
                field->field[col][row] = card;
            }
        }
    }
}




// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Print field content to screen
 * Draws the field with cards and borders
 */
static void print_field(const void *field_pointer, Screen *screen, const Cursor *hovered_card) {
    const Field *field = (const Field *)field_pointer;
    int contentHeight = 2 * FIELD_HEIGHT - 1 + CARD_HEIGHT - 2;
    int contentWidth = SCREEN_WIDTH - BORDER_OFFSET_X - 1;

    fill_area(screen, DECK_OFFSET + BORDER_OFFSET_Y, BORDER_OFFSET_X, contentHeight, contentWidth, ' ', ((Color){0x0030992e}), COLOR_NONE);

    int y_offset_base = DECK_OFFSET + BORDER_OFFSET_Y;
    int hovered_x = hovered_card->coords.x;
    int hovered_y = hovered_card->coords.y;

    for (int y = 0; y < FIELD_HEIGHT; ++y) {
        int y_0 = y * CARD_COVERED_HEIGHT + y_offset_base;
        int is_hovered_row = y > hovered_y;

        for (int x = 0; x < FIELD_WIDTH; ++x) {
            int additional_offset = (is_hovered_row && x == hovered_x && hovered_card->subject == field_pointer);
            int x_0 = x * CARD_WIDTH + BORDER_OFFSET_X;

            const Card *current_card = field->field[y][x];

            if (y == 0 && !current_card) {
                add_borders(screen, y_offset_base, x_0, CARD_HEIGHT, CARD_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);
            }
            else {
                print_card(screen, current_card, y_0 + additional_offset, x_0, CARD_HEIGHT, CARD_WIDTH);
            }
        }
    }
}




// =========================
//  CURSOR_INTERACTABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Place cursor in field
 * Calculates target coordinates based on cursor position and updates them
 */
static Coords place_cursor_in_field(const void *field_pointer, const Coords cursor_coords) {
    const Field *field = (const Field *)field_pointer;
    int part = BORDER_OFFSET_Y + 1 + cursor_coords.y * 2 + BORDER_OFFSET_Y;
    bool is_last_card = FIELD_HEIGHT == cursor_coords.y || !field->field[cursor_coords.y + 1][cursor_coords.x];

    return (Coords) {
        .y = (short)(part + (is_last_card ? CARD_HEIGHT : CARD_COVERED_HEIGHT + 1) + CARD_HEIGHT),
        .x = (short)(BORDER_OFFSET_X - 1 + cursor_coords.x * CARD_WIDTH + (CARD_WIDTH / 2))
    };
}

/*
 * Move cursor in field
 * Updates cursor coordinates based on delta and ensures they are within valid bounds
 */
static void move_in_field(const void *field_pointer, Coords *coords, const Coords delta) {
    short new_x = coords->x + delta.x;
    short new_y = coords->y + delta.y;
    const Field *field = (const Field *)field_pointer;

    coords->x = (short)((new_x + FIELD_WIDTH) % FIELD_WIDTH);
    if (new_y >= 0 && new_y < FIELD_HEIGHT && field->field[new_y][coords->x] && !field->field[new_y][coords->x]->hidden) coords->y = new_y;
    if (delta.y == 0) coords->y = (short)get_last_card_y(field, coords->x);
}

/*
 * Get default coords
 * Gets the default coords of the field
 */
static Coords get_default_coords(const void *field_pointer) {
    const Field *field = (const Field *)field_pointer;
    return (Coords) {.x = 3, .y = (short)get_last_card_y(field, 3)};
}

/*
 * Get cursor config
 * Gets the cursor config of the field
 */
static CursorConfig get_cursor_config_in_field(const void *field_pointer, const Coords cursor_coords) {
    (void)field_pointer;
    (void)cursor_coords;
    return (CursorConfig) {
        .type = CURSOR_UP_WIDE,
        .background = COLOR_NONE,
        .foreground = COLOR_WHITE,
        .effect = Effect_Bold,
    };
}




// =========================
//  CARD_HANDLER INTERFACE IMPLEMENTATION
// =========================
/*
 * Get cards from field
 * Removes selected cards from field and marks them as not selected
 */
static void get_cards_in_field(void *field_pointer, const Container *container) {
    Field *field = (Field *)field_pointer;

    for (int i = 0; i < container->size; i++) {
        Card *card = (Card *)container_get_element(container, i);
        card->selected = false;
        field->field[card->coords.y][card->coords.x] = NULL;
        if (card->coords.y - 1 >= 0 && field->field[card->coords.y - 1][card->coords.x]) 
            field->field[card->coords.y - 1][card->coords.x]->hidden = false;
    }
}

/*
 * Select cards in field
 * Adds selected cards to container and marks them as selected
 */
static void select_cards_in_field(void *field_pointer, Coords cursor_coords, Container *container) {
    Field *field = (Field *)field_pointer;
    // If container is not empty, clear it
    if (!container_is_empty(container)) {
        while (!container_is_empty(container)) {
            ((Card *)container_pop_element(container))->selected = false;
        }
        return;
    }

    // Select cards in field
    for (int i = cursor_coords.y; i < FIELD_HEIGHT && field->field[i][cursor_coords.x]; ++i) {
        Card *current_card = field->field[i][cursor_coords.x];
        const Card *next_card_in_row = (i + 1 < FIELD_HEIGHT) ? field->field[i + 1][cursor_coords.x] : NULL;

        // If move is valid, select card
        if (!next_card_in_row ||
            (current_card->numeral - next_card_in_row->numeral == 1 && 
            current_card->suit - next_card_in_row->suit % 2 != 0)) {
            current_card->selected = true;
            container_add_element(container, current_card);
        }
        else {
            // If move is not valid, clear container
            while (!container_is_empty(container)) {
                ((Card *)container_pop_element(container))->selected = false;
            }
            return;
        }
    }
    container_set_source(container, field_pointer);
}

/*
 * Check if cursor can place cards in field
 * Checks if move is valid
 */
static bool can_place_in_field(const void *field_pointer, const Coords cursor_coords, const Container *container) {
    const Field *field = (const Field *)field_pointer;

    if (cursor_coords.y + container->size >= FIELD_HEIGHT) return false;
    if (field->field[cursor_coords.y + 1][cursor_coords.x] != NULL) return false;
    if (field->field[cursor_coords.y][cursor_coords.x] != NULL && field->field[cursor_coords.y][cursor_coords.x]->selected) return false;

    const Card *card = (Card *)container_get_element(container, 0);
    const Card *target_card = field->field[cursor_coords.y][cursor_coords.x];
    if (target_card == NULL) {
        if (card->numeral != King) return false;
    }
    else {
        if (target_card->suit % 2 == card->suit % 2) return false;
        if (target_card->numeral - card->numeral != 1) return false;
    }
    
    return true;
}

/*
 * Place cards in field
 * Places selected cards in field at specified cursor coordinates
 */
static void place_cards_in_field(void *field_pointer, Coords cursor_coord, Container *container) {
    Field *field = (Field *)field_pointer;
    bool y_offset = (field->field[cursor_coord.y][cursor_coord.x]);

    while (!container_is_empty(container)) {
        Card *card = container_pop_element(container);
        card->object = Field_enum;
        card->coords.x = cursor_coord.x;
        card->coords.y = (short)(cursor_coord.y + y_offset + container->size);
        field->field[cursor_coord.y + y_offset + container->size][cursor_coord.x] = card;
    }
}

/*
 * Check if cursor is on the same card in field
 * Compares cursor position to field card
 */
static bool is_same_card_in_field(const void *field_pointer, const Coords cursor_coords, const void *card) {
    const Field *field = (const Field *)field_pointer;
    return field->field[cursor_coords.y][cursor_coords.x] == (Card *)card;
}




// =========================
//  POSITION_HANDLER INTERFACE IMPLEMENTATION
// =========================
/*
 * Save current position in field
 * Sets current coordinates to restore coordinates
 */
static void save_current_pos_in_field(void *field_pointer, Coords current_coords) {
    Field *field = (Field *)field_pointer;
    SET_RESTORE_COORDS(field, current_coords);
}

/*
 * Restore position in field
 * Sets restore coordinates to current coordinates and calculates last card y position
 */
static void restore_pos_in_field(void *field_pointer, Coords *current_coords) {
    Field *field = (Field *)field_pointer;
    current_coords->x = GET_RESTORE_COORDS(field).x;
    current_coords->y = (short)get_last_card_y(field, current_coords->x);
}




// =========================
//  CARD_PROVIDER INTERFACE IMPLEMENTATION
// =========================
/*
 * Peek at top card in field column
 * Returns the last card in the specified column
 */
static Card *field_peek(void *field_pointer, Coords coords) {
    (void)coords;
    Field *field = (Field *)field_pointer;

    return field->field[get_last_card_y(field, coords.x)][coords.x];
}

/*
 * Remove card from field
 * Uncovers the card below if present
 */
static void field_pop(void *field_pointer, Card *card) {
    Field *field = (Field *)field_pointer;
    Coords coords = card->coords;

    field->field[coords.y][coords.x] = NULL;
    if (coords.y - 1 >= 0 && field->field[coords.y - 1][coords.x]) {
        field->field[coords.y - 1][coords.x]->hidden = false;
    }
}




/*
 * Initialize field with cards from deck
 * Sets up field structure and interfaces
 */
Field *init_field(Arena *arena) {
    Field *field = (Field *)arena_alloc(arena, sizeof(Field));

    INTERFACES(arena, field, {
        DRAWABLE(print_field);
        CURSOR_INTERACTABLE(place_cursor_in_field, move_in_field, get_default_coords, get_cursor_config_in_field);
        POSITION_HANDLER(save_current_pos_in_field, restore_pos_in_field);
        CARD_HANDLER({
            CAN_GIVE_CARDS(select_cards_in_field, is_same_card_in_field, get_cards_in_field);
            CAN_TAKE_CARDS(can_place_in_field, place_cards_in_field);
        });
        CORE_DEPENDENT();
    });

    
    static CardProvider card_provider = (CardProvider) {
        .peek = field_peek,
        .pop  = field_pop,
    };  

    static FieldMethods field_methods = (FieldMethods) {
        .card_provider = &card_provider,
    };

    field->export_methods = &field_methods;

    return field;
}

