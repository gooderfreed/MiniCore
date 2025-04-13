#ifndef DEMO_H
#define DEMO_H

#include "zen.h"

/*
 * Sinple Demo structure
 * Represents a demo object
 */
typedef struct Demo {
    ObjectInterfaces interfaces;
    char *my_string;
} Demo;

#endif
