#include "../inc/demo.h"

// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Draw demo on screen
 */
static void print_demo(const void *demo_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Demo *demo = (Demo *)demo_pointer;

    insert_text(screen, 3, 3, "Hello",               COLOR_RED,    COLOR_BLACK, Effect_Blink);
    insert_text(screen, 4, 3, "from",                COLOR_RED,    COLOR_BLACK, Effect_Underline);
    insert_text(screen, 5, 3, demo->interfaces.name, COLOR_GREEN,  COLOR_BLACK, Effect_Italic);
    insert_text(screen, 6, 1, demo->my_string,       COLOR_YELLOW, COLOR_BLACK, Effect_Bold);
}




/*
 * Initialize demo
 * This function initializes the demo object
 */
static Demo *init_demo(Arena *arena) {
    // Allocate memory for the demo object
    Demo *demo = (Demo *)arena_alloc(arena, sizeof(Demo));

    // Set the demo's string
    demo->my_string = "Mystring";

    // Initialize the demo's interfaces
    INTERFACES(arena, demo, {
        DRAWABLE(print_demo);
    });

    // Return the initialized demo object
    return demo;
}




/*
 * Prepare screen
 */
static void layer_prepare_screen(Screen *screen) {
    add_borders(screen, 0, 0, 10, 10, COLOR_BLUE, COLOR_WHITE, L"-|++++++");
}


/*
 * Main loop
 * This function handles user input and updates the core
 */
static void layer_loop(Core *core, wint_t key) {
    switch (key) {
        case L'q': case L'й': core_shutdown(core); exit(0); break;
    }
}

/*
 * Main function
 * This function initializes the core, screen, map, and other necessary components.
 * It also sets up the game loop and handles user input.
 */
// #define ARENA_SIZE 3732
// #define ARENA_SIZE 3268
// #define ARENA_SIZE 3172
#define ARENA_SIZE 3076
int main(void) {
    // Initialize the memory arena
    size_t buffer[ARENA_SIZE]; // 4KB | demo requires 3800 bytes
    Arena *arena = arena_new_static(buffer, ARENA_SIZE);
    // Initialize the core
    Core  *core  = core_init(arena);

    // Initialize the screen
    Screen *screen = init_screen(arena, 10, 10, COLOR_BLACK, COLOR_WHITE, ' ');
    core_set_screen(core, screen);
    
    // Initialize the map
    Map *map = init_map(arena, 1, (Coords){.x = 0, .y = 0, .z = 0});
    
    // Initialize the map layer
    MapLayer *layer = NULL;
    MAP_LAYER(arena, layer, {
        prepare_screen = layer_prepare_screen;
        loop = layer_loop;
    }, {
        OBJECT(init_demo(arena), COORDS(0, 0));
    });

    // Set the map layer
    map_set_layer(map, layer, 0);
    // Set the map in the core
    core_set_map(core, map);


    // Set the target FPS
    core_set_target_fps(core, 10);
    // Set the ticks per second
    core_set_ticks_per_second(core, 10);

    // Game loop
    while (!core_should_close(core)) {
        // Handle user input
        if (core_has_input()) {
            wint_t ch = getwchar();

            // Shutdown the core if the user presses ESC
            if (ch == KEY_ESC) {
                core_shutdown(core);
                break;
            }

            // Manage the game loop
            core_manage_loop(core, ch);
        }
    }

    return 0;
}
