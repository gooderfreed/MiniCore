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

/*
 * Map implementation
 * Handles map object management and cursor movement
 */
#include "../inc/core.h"

/*
 * Move cursor on map to new position
 * Validates move and checks if target position is interactable
 * Does nothing if move is invalid or target is not interactable
 */
void map_move(Map *map, Coords move) {
    if (move.x == 0 && move.y == 0) return;

    Coords new_coords = {
        .x = map->global_coords.x + move.x,
        .y = map->global_coords.y + move.y,
    };

    if (new_coords.x < 0 || new_coords.x >= MAP_WIDTH ||
        new_coords.y < 0 || new_coords.y >= MAP_HEIGHT) {
        return;
    }

    MapObject object = map->objects[new_coords.y][new_coords.x];
    if (!(object.object && INTERACTABLE(object.object))) return;

    map->global_coords = new_coords;
}

/*
 * Get object at current cursor position
 * Returns MapObject structure containing object data
 */
MapObject map_get_current_object(Map *map) {
    return map->objects[map->global_coords.y][map->global_coords.x];
} 

/*
 * Get object at position
 * Returns MapObject structure containing object data
 */
MapObject map_get_object(Map *map, Coords coords) {
    return map->objects[coords.y][coords.x];
} 
