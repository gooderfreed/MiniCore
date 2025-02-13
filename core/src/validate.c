/*
 * Validate implementation
 * Handles object interface validation
 */
#include "../inc/core.h"

/*
 * Validate Drawable interface implementation
 * Checks if interface pointer and required functions are present
 * Prints error message if validation fails
 */
static bool validate_drawable(const void *interface, const char *name) {
    const Drawable *drawable = interface;
    if (!drawable) {
        wprintf(L"Error in %s: Drawable interface is NULL\n", name);
        return false;
    }

    if (!drawable->print) {
        wprintf(L"Error in %s: Drawable interface is missing print function\n", name);
        return false;
    }
    return true;
}

/*
 * Validate Interactable interface implementation
 * Checks if interface has required movement and cursor placement functions
 * Prints detailed error messages for missing components
 */
static bool validate_cursor_interactable(const void *interface, const char *name) {
    const CursorInteractable *cursor_interactable = interface;
    if (!cursor_interactable) {
        wprintf(L"Error in '%s': CursorInteractable interface is NULL\n", name);
        return false;
    }

    if (!cursor_interactable->move_cursor) {
        wprintf(L"Error in '%s': CursorInteractable interface is missing 'move_cursor' function\n", name);
        return false;
    }

    if (!cursor_interactable->place_cursor) {
        wprintf(L"Error in '%s': CursorInteractable interface is missing 'place_cursor' function\n", name);
        return false;
    }

    if (!cursor_interactable->get_default_coords) {
        wprintf(L"Error in '%s': CursorInteractable interface is missing 'get_default_coords' function\n", name);
        return false;
    }

    if (!cursor_interactable->get_cursor_config) {
        wprintf(L"Error in '%s': CursorInteractable interface is missing 'get_cursor_config' function\n", name);
        return false;
    }

    return true;
}

/*
 * Validate CardHandler interface implementation
 * Ensures all required card manipulation functions are present
 * Validates both card giving and taking capabilities separately
 */
static bool validate_card_handler(const void *interface, const char *name) {
    const CardHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error in '%s': CardHandler interface is NULL\n", name);
        return false;
    }

    if (handler->can_give_cards) {
        if (!handler->select_cards) {
            wprintf(L"Error in '%s': Object declares can_give_cards but 'select_cards' function are missing\n", name);
            return false;
        }

        if (!handler->get_cards) {
            wprintf(L"Error in '%s': Object declares can_give_cards but 'get_cards' function are missing\n", name);
            return false;
        }

        if (!handler->is_same_card) {
            wprintf(L"Error in '%s': Object declares can_give_cards but 'is_same_card' function are missing\n", name);
            return false;
        }
    }

    if (handler->can_take_cards) {
        if (!handler->can_place) {
            wprintf(L"Error in '%s': Object declares can_take_cards but 'can_place' function are missing\n", name);
            return false;
        }

        if (!handler->place_cards) {
            wprintf(L"Error in '%s': Object declares can_take_cards but 'place_cards' function are missing\n", name);
            return false;
        }
    }
    return true;
}

/*
 * Validate ButtonHandler interface implementation
 */
static bool validate_button_handler(const void *interface, const char *name) {
    const ButtonHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error in '%s': ButtonHandler interface is NULL\n", name);
        return false;
    }

    return true;
}

/*
 * Validate PositionHandler interface implementation
 * Checks for required position restoration and saving functions
 * Reports missing function implementations
 */
static bool validate_position_handler(const void *interface, const char *name) {
    const PositionHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error in '%s': PositionHandler interface is NULL\n", name);
        return false;
    }

    if (!handler->restore_pos) {
        wprintf(L"Error in '%s': PositionHandler interface is missing 'restore_pos' function\n", name);
        return false;
    }

    if (!handler->save_current_pos) {
        wprintf(L"Error in '%s': PositionHandler interface is missing 'save_current_pos' function\n", name);
        return false;
    }

    return true;
}

/*
 * Validate Dynamic interface implementation
 * Ensures object cleanup function is properly defined
 * Required for objects that need resource deallocation
 */
static bool validate_dynamic(const void *interface, const char *name) {
    const Dynamic *dynamic = interface;
    if (!dynamic) {
        wprintf(L"Error in '%s': Dynamic interface is NULL\n", name);
        return false;
    }

    if (!dynamic->free) {
        wprintf(L"Error in '%s': Dynamic interface is missing 'free' function\n", name);
        return false;
    }

    return true;
}

/*
 * Validate Updateable interface implementation
 * Ensures object update function is properly defined
 * Required for objects that need periodic updates
 */
static bool validate_updateable(const void *interface, const char *name) {
    const Updateable *updateable = interface;
    if (!updateable) {
        wprintf(L"Error in '%s': Updateable interface is NULL\n", name);
        return false;
    }

    if (!updateable->update) {
        wprintf(L"Error in '%s': Updateable interface is missing 'update' function\n", name);
        return false;
    }

    return true;
}

/*
 * Validate all interfaces for game object
 * Runs appropriate validators based on object capabilities
 * Returns false if any required interface validation fails
 */
bool validate_object_interfaces(const ObjectInterfaces *interfaces) {
    InterfaceValidator validators[] = {
        VALIDATOR(is_drawable,     drawable,                   validate_drawable),
        VALIDATOR(is_cursor_interactable, cursor_interactable, validate_cursor_interactable),
        VALIDATOR(can_hold_cards,  card_handler,               validate_card_handler),
        VALIDATOR(have_buttons,    button_handler,             validate_button_handler),
        VALIDATOR(is_positionable, position_handler,           validate_position_handler),
        VALIDATOR(is_dynamic,      dynamic,                    validate_dynamic),
        VALIDATOR(requires_update, updateable,                 validate_updateable)
    };

    for (size_t i = 0; i < sizeof(validators) / sizeof(validators[0]); i++) {
        const InterfaceValidator *v = &validators[i];
        if (v->capability_flag && !v->validate(v->interface, interfaces->name)) {
            return false;
        }
    }

    return true;
} 
