#ifndef DICT_H_
# define DICT_H_

#include <stdbool.h>

/**
 * @defgroup Dict Dict
 * @{
 */

/**
 * A dictionnary entry
 */
typedef struct wsky_DictEntry_s wsky_DictEntry;

/**
 * A dictionnary which maps keys to values
 */
typedef struct wsky_Dict_s {
  /** Private member, don't use it */
  wsky_DictEntry *first;

} wsky_Dict;

/**
 * Initializes a dictionnary.
 * The returned dictionnary must be freed with wsky_Dict_free().
 */
void wsky_Dict_init(wsky_Dict *self);

/**
 * Returns a pointer to a new malloc'd dictionnary.
 * The returned dictionnary must be freed with wsky_Dict_delete().
 */
wsky_Dict *wsky_Dict_new(void);

/**
 * Frees the members of a dictionnary initialized with wsky_Dict_init().
 * Does not call free() on the given pointer.
 * Does not call free() the elements of the dictionnary. You can free them
 * manually with wsky_Dict_apply().
 */
void wsky_Dict_free(wsky_Dict *self);

/**
 * Frees a malloc'd dictionnary initialized with wsky_Dict_new().
 * Acts like the C++ `delete` keyword: calls wsky_Dict_free() and then free()
 * on the given pointer.
 * Does not call free() the elements of the dictionnary. You can free them
 * manually with wsky_Dict_apply().
 */
void wsky_Dict_delete(wsky_Dict *self);

/**
 * Applies a function on each element of the dictionnary.
 */
void wsky_Dict_apply(wsky_Dict *self,
                     void (*function)(const char *key, void *value));

/**
 * Returns `true` if the dictionnary contain an entry with the given key.
 */
bool wsky_Dict_contains(const wsky_Dict *self, const char *key);

/**
 * Sets a value to a given key.
 * Creates a new entry if the value does not exists.
 */
void wsky_Dict_set(wsky_Dict *self, const char *key, void *value);

/**
 * Returns the value for a key.
 * Returns NULL if there is no entry with the given key.
 */
void *wsky_Dict_get(wsky_Dict *self, const char *key);

/**
 * Removes an entry from a dictionnary.
 * Returns NULL if there is no entry with the given key.
 */
void *wsky_Dict_remove(wsky_Dict *self, const char *key);

/**
 * @}
 */

#endif /* !DICT_H_ */
