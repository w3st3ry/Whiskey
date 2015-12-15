#include "dict.h"

#include <stdlib.h>

struct wsky_DictEntry_s {
  const char *key;
  void *value;
  struct wsky_DictEntry_s *previous;
  struct wsky_DictEntry_s *next;
};

typedef wsky_Dict Dict;
typedef wsky_DictEntry Entry;



void wsky_Dict_init(Dict *this) {
  this->first = NULL;
}

Dict *wsky_Dict_new(void) {
  Dict *this = malloc(sizeof(Dict));
  wsky_Dict_init(this);
  return this;
}

void wsky_Dict_free(Dict *this) {
  Entry *entry = this->first;
  while (entry) {
    Entry *next = entry->next;
    free(entry);
    entry = next;
  }
}

void wsky_Dict_delete(Dict *this) {
  wsky_Dict_free(this);
  free(this);
}



void wsky_Dict_apply(Dict *this,
                     void (*function)(const char *key, void *value)) {

  Entry *entry = this->first;
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

static Entry *getEntry(const Dict *this, const char *key) {
  Entry *entry = this->first;
  while (entry) {
    if (stringEquals(key, entry->key)) {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

bool wsky_Dict_contains(const Dict *this, const char *key) {
  return (getEntry(this, key) != NULL);
}

static Entry *newEntry(const char *key, void *value,
                       Entry *previous, Entry *next) {
  Entry *entry = malloc(sizeof(Entry));
  entry->key = key;
  entry->value = value;
  entry->previous = previous;
  entry->next = next;
  if (previous)
    previous->next = entry;
  if (next)
    next->previous = entry;
  return entry;
}

static void add(Dict *this, const char *key, void *value) {
  Entry *entry = newEntry(key, value, NULL, this->first);
  this->first = entry;
}

void wsky_Dict_set(Dict *this, const char *key, void *value) {
  Entry *entry = getEntry(this, key);
  if (entry) {
    entry->value = value;
  } else {
    add(this, key, value);
  }
}

void *wsky_Dict_get(wsky_Dict *this, const char *key) {
  Entry *entry = getEntry(this, key);
  if (!entry)
    return NULL;
  return entry->value;
}

void *wsky_Dict_remove(wsky_Dict *this, const char *key) {
  Entry *entry = getEntry(this, key);
  if (!entry)
    return NULL;

  Entry *previous = entry->previous;
  Entry *next = entry->next;
  if (this->first == entry) {
    this->first = next;
  }
  if (next)
    next->previous = previous;
  if (previous)
    previous->next = next;
  void *value = entry->value;
  free(entry);
  return value;
}
