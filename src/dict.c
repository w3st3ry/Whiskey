#include "dict.h"

#include <stdlib.h>
#include <string.h>
#include "gc.h"


struct wsky_DictEntry_s {
  char *key;
  void *value;
  struct wsky_DictEntry_s *previous;
  struct wsky_DictEntry_s *next;
};

typedef wsky_Dict Dict;
typedef wsky_DictEntry Entry;



void wsky_Dict_init(Dict *self) {
  self->first = NULL;
}

Dict *wsky_Dict_new(void) {
  Dict *self = wsky_safeMalloc(sizeof(Dict));
  wsky_Dict_init(self);
  return self;
}

void wsky_Dict_free(Dict *self) {
  Entry *entry = self->first;
  while (entry) {
    Entry *next = entry->next;
    wsky_free(entry->key);
    wsky_free(entry);
    entry = next;
  }
  self->first = NULL;
}

void wsky_Dict_delete(Dict *self) {
  wsky_Dict_free(self);
  wsky_free(self);
}



void wsky_Dict_apply(Dict *self,
                     void (*function)(const char *key, void *value)) {

  Entry *entry = self->first;
  while (entry) {
    function(entry->key, entry->value);
    entry = entry->next;
  }
}

void wsky_Dict_applyConst(const Dict *self,
                          void (*function)(const char *key, void *value)) {

  Entry *entry = self->first;
  while (entry) {
    function(entry->key, entry->value);
    entry = entry->next;
  }
}


static bool stringEquals(const char *a, const char *b) {
  while (*a == *b) {
    if (!*a) {
      return true;
    }
    a++;
    b++;
  }
  return false;
}

static Entry *getEntry(const Dict *self, const char *key) {
  Entry *entry = self->first;
  while (entry) {
    if (stringEquals(key, entry->key)) {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

bool wsky_Dict_contains(const Dict *self, const char *key) {
  return (getEntry(self, key) != NULL);
}

static Entry *newEntry(const char *key, void *value,
                       Entry *previous, Entry *next) {
  Entry *entry = wsky_safeMalloc(sizeof(Entry));
  entry->key = wsky_strdup(key);
  entry->value = value;
  entry->previous = previous;
  entry->next = next;
  if (previous)
    previous->next = entry;
  if (next)
    next->previous = entry;
  return entry;
}

static void add(Dict *self, const char *key, void *value) {
  Entry *entry = newEntry(key, value, NULL, self->first);
  self->first = entry;
}

void wsky_Dict_set(Dict *self, const char *key, void *value) {
  Entry *entry = getEntry(self, key);
  if (entry) {
    entry->value = value;
  } else {
    add(self, key, value);
  }
}

void *wsky_Dict_get(wsky_Dict *self, const char *key) {
  Entry *entry = getEntry(self, key);
  if (!entry)
    return NULL;
  return entry->value;
}

void *wsky_Dict_remove(wsky_Dict *self, const char *key) {
  Entry *entry = getEntry(self, key);
  if (!entry)
    return NULL;

  Entry *previous = entry->previous;
  Entry *next = entry->next;
  if (self->first == entry) {
    self->first = next;
  }
  if (next)
    next->previous = previous;
  if (previous)
    previous->next = next;
  void *value = entry->value;
  wsky_free(entry->key);
  wsky_free(entry);
  return value;
}
