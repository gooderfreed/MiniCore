/*
 * Deck implementation
 * Handles deck object management and card drawing
 */
#include "../../inc/solitaire.h"

/*
 * Shuffle deck array using Fisher-Yates algorithm
 * Randomly permutes all cards in the deck
 */
static void deck_shuffle(Deck *deck) {
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        Card temp = deck->deck[i];
        deck->deck[i] = deck->deck[j];
        deck->deck[j] = temp;
    }
    
    deck->pointer = &deck->deck[0];
}

/*
 * Reset deck to initial state
 * Resets all cards in the deck to default state
 */
void deck_reset(Deck *deck) {
    for (int i = 0; i < DECK_SIZE; i++) {
        deck->deck[i].object = Deck_enum;
        deck->deck[i].selected = false;
        deck->deck[i].hidden = false;
        deck->deck[i].coords = (Coords) {.x = 1, .y = 0};
    }
    deck->pointer = &deck->deck[0];
    deck_shuffle(deck);
}

/*
 * Move to next card in deck
 * Cycles through deck until finding next available card
 */
static void next_card(Deck *deck) {
    const Card *start = deck->pointer;
    if (!start) return;

    do {
        deck->pointer++;

        if (deck->pointer >= &deck->deck[DECK_SIZE]) deck->pointer = &deck->deck[0];
        if (deck->pointer == start) {
            if (deck->pointer->object != Deck_enum)
                deck->pointer = NULL;
            break;
        }
    } while (deck->pointer->object != Deck_enum);
}

/*
 * Remove current card from deck
 * Returns removed card and advances to next card
 */
static Card *draw_card(Deck *deck) {
    Card *card = deck->pointer;

    card->object = Unknown;
    next_card(deck);

    return card;
}

/*
 * Draw deck on screen
 * Shows deck pile and current top card
 */
static void print_deck(const void *deck_pointer, Screen *screen, const Cursor *cursor) {
    const Deck *deck = (const Deck *)deck_pointer;
    (void) cursor;
    fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, DECK_OFFSET - 1, 2 * CARD_WIDTH, L' ', COLOR_GREEN, COLOR_RESET);
    if (deck->pointer) {
        print_card(screen, deck->pointer, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH);
        fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT - 1, CARD_WIDTH, L'░', COLOR_RESET, COLOR_RESET);
        add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT, CARD_WIDTH, COLOR_BRIGHT_BLACK, COLOR_WHITE, card_border);
    }
    else {
        add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH, COLOR_GREEN, COLOR_WHITE, fat_border);
        add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT, CARD_WIDTH, COLOR_GREEN, COLOR_WHITE, fat_border);
    }
}

/*
 * Handle cursor movement in deck area
 * Restricts movement to horizontal only between deck and current card
 */
static void move_in_deck(const void *deck_pointer, Coords *coords, const Coords delta) {
    (void)deck_pointer;
    
    if (delta.y != 0) return;
    
    short new_x = coords->x + delta.x;
    if (new_x < 0 || new_x > 1) return;
    
    coords->x = new_x;
}

/*
 * Position cursor relative to deck elements
 * Places cursor at bottom of deck or current card
 */
static void place_cursor_in_deck(const void *deck_pointer, const Coords cursor_coords, Coords *target_coords) {
    (void)deck_pointer;
    
    target_coords->y = BORDER_OFFSET_Y + CARD_HEIGHT;
    target_coords->x = (short)(cursor_coords.x * CARD_WIDTH + (CARD_WIDTH / 2) + BORDER_OFFSET_X - 1);
}

/*
 * Get default coords
 * Gets the default coords of the deck
 */
static Coords get_default_coords(const void *deck_pointer) {
    (void)deck_pointer;
    return (Coords) {.x = 0, .y = 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the deck
 */
static CursorConfig get_cursor_config_in_deck(const void *deck_pointer, const Coords cursor_coords) {
    (void)deck_pointer;
    (void)cursor_coords;
    return (CursorConfig) {.type = CURSOR_UP_WIDE};
}

/*
 * Handle next card button click
 * Advances to next card in deck and clears cursor container
 */
static void handle_next_card_button(void *deck_pointer, void *context) {
    next_card((Deck *)deck_pointer);
    Container *container = (Container *)context;
    if (container->size > 0 && ((Deck *)deck_pointer)->pointer) {
        while (!container_is_empty(container)) {
            Card *card = (Card *)container_pop_element(container);
            card->selected = false;
        }
    }
}

/*
 * Select card in deck
 * Adds selected card to container and marks it as selected
 */
static void select_card_in_deck(void *deck_pointer, const Coords cursor_coords, Container *container) {
    Deck *deck = (Deck *)deck_pointer;
    (void)cursor_coords;

    if (deck->pointer == NULL) return;

    if (container->size == 0) {
        deck->pointer->selected = true;
        container_add_element(container, deck->pointer);
        container_set_source(container, deck_pointer);
    } else {
        deck->pointer->selected = false;
        container_pop_element(container);
    }
}

/*
 * Get card from deck
 * Removes selected card from deck and advances to next card
 */
static void get_card_in_deck(void *deck_pointer, const Container *container) {
    Deck *deck = (Deck *)deck_pointer;
    (void)container;

    deck->pointer->selected = false;
    deck->pointer->object = Unknown;
    next_card(deck);
}

/*
 * Check if cursor is on the same card in deck
 * Compares cursor position to deck pointer
 */
static bool is_same_card_in_deck(const void *deck_pointer, const Coords cursor_coords, const Card *card) {
    (void)cursor_coords;
    const Deck *deck = (const Deck *)deck_pointer;
    return deck->pointer == card;
}

static Card *deck_peek(void *deck_pointer, Coords coords) {
    (void)coords;
    Deck *deck = (Deck *)deck_pointer;

    return deck->pointer;
}

static void deck_pop(void *deck_pointer, Card *card) {
    (void)card;
    Deck *deck = (Deck *)deck_pointer;

    next_card(deck);
}

/*
 * Generate a deck of cards
 * Initializes deck structure and fills it with cards
 */
Deck generate_deck(void) {
    Deck deck = {0};
    
    int i = 0;
    for (Suit suit = Spades; suit < CARD_SUITS; suit++) {
        for (Numeral numeral = Ace; numeral <= CARD_NUMERALS; numeral++) {
            Card card = {
                .suit = suit,
                .numeral = numeral,
                .selected = false,
                .object = Deck_enum,
                .coords = {.x = 1, .y = 0}
            };
            deck.deck[i++] = card;
        }
    }
    deck.pointer = &deck.deck[0];

    static Drawable drawable = {
        .is_active = true,
        .print = print_deck
    };

    static const CursorInteractable cursor_interactable = {
        .place_cursor        = place_cursor_in_deck,
        .move_cursor         = move_in_deck,
        .get_default_coords  = get_default_coords,
        .get_cursor_config   = get_cursor_config_in_deck
    };

    static const CardHandler card_handler = {
        .can_give_cards = true,
        .select_cards   = select_card_in_deck,
        .get_cards      = get_card_in_deck,
        .is_same_card   = is_same_card_in_deck,
        
        .can_take_cards = false,
        .place_cards    = NULL,
        .can_place      = NULL
    };
    
    static Button next_card_button = {
        .coords = {.x = 0, .y = 0},
        .on_click = handle_next_card_button,
    };

    static ButtonHandler button_handler = {
        .buttons_count = 1,
        .buttons = {
            [0] = &next_card_button
        },
    };

    deck.interfaces = (ObjectInterfaces) {
        .name           = "Deck",
        .capabilities = {
            .can_hold_cards         = true,
            .have_buttons           = true,
            .is_drawable            = true,
            .is_cursor_interactable = true,
        },
        .card_handler        = &card_handler,
        .button_handler      = &button_handler,
        .drawable            = &drawable,
        .cursor_interactable = &cursor_interactable,
    };


    static CardProvider card_provider = (CardProvider) {
        .peek = deck_peek,
        .pop  = deck_pop,
    };  

    static DeckMethods deck_methods = (DeckMethods) {
        .card_provider = &card_provider,
        .draw_card = draw_card,
    };

    deck.export_methods = &deck_methods;

    deck_shuffle(&deck);
    return deck;
}
