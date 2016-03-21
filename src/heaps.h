#ifndef HEAP_H
# define HEAP_H

# include "whiskey_private.h"

bool wsky_heaps_contains(void *pointer);

/**
 * Allocates an object.
 *
 * Never returns NULL.
 *
 * @param className The class name, for debugging purposes only.
 */
Object *wsky_heaps_allocateObject(const char *className);

void wsky_heaps_unmark(void);

void wsky_heaps_deleteUnmarkedObjects(void);

void wsky_heaps_freeObject(Object *object);

/**
 * Frees everything.
 */
void wsky_heaps_free(void);


#endif /* HEAP_H */
