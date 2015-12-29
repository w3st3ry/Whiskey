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
 * A dictionnary
 */
typedef struct wsky_Dict_s {
  wsky_DictEntry *first;
} wsky_Dict;

void wsky_Dict_init(wsky_Dict *self);
wsky_Dict *wsky_Dict_new(void);

void wsky_Dict_free(wsky_Dict *self);
void wsky_Dict_delete(wsky_Dict *self);

void wsky_Dict_apply(wsky_Dict *self,
                     void (*function)(const char *key, void *value));
bool wsky_Dict_contains(const wsky_Dict *self, const char *key);
void wsky_Dict_set(wsky_Dict *self, const char *key, void *value);
void *wsky_Dict_get(wsky_Dict *self, const char *key);
void *wsky_Dict_remove(wsky_Dict *self, const char *key);

/**
 * @}
 */
#endif /* !DICT_H_ */
