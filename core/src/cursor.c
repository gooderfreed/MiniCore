/*
 * Cursor implementation
 * Handles cursor state and visualization
 */
#include "../inc/core.h"


/*
 * Initialize cursor with starting object and position
 * Returns cursor structure ready for use
 */
Cursor *init_cursor(Arena *arena, void *start_object, Coords start_coords, Container *cursor_cards) {
    Cursor *cursor = (Cursor *)arena_alloc(arena, sizeof(Cursor));

    *cursor = (Cursor) {
        .coords = start_coords,
        .cards = cursor_cards,
        .subject = start_object,
    };

    return cursor;
}

/*
 * Print cursor on screen
 * Handles cursor visualization based on current state and position
 */
void print_cursor(Cursor *cursor, Screen *screen) {
    // If subject is interactable, let it place the cursor
    if (cursor && IS_CURSOR_INTERACTABLE(cursor->subject)) {
        // Get base coordinates for cursor
        Coords base_coords = {
            .x = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2), 
            .y = CARD_HEIGHT
        };

        PLACE_CURSOR(cursor->subject, cursor->coords, &base_coords);

        CursorConfig config = GET_CURSOR_CONFIG(cursor->subject, cursor->coords);
        // if (config.type != CURSOR_CUSTOM) {
        //     if (config.foreground == COLOR_UNDEFINED) config.foreground = COLOR_NONE;
        // }

        // If subject has custom cursor, let it draw it
        if (config.type == CURSOR_CUSTOM) {
            CUSTOM_DRAW(cursor->subject, cursor, screen, base_coords);
            return;
        }

        // Otherwise, draw default cursor
        screen_draw_cursor(screen, base_coords, config);
    }
    
}

