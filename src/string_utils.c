#include "string_utils.h"

#include <string.h>
#include "gc.h"

char *wsky_strdup(const char *string) {
  size_t length = strlen(string);
  char *newString = wsky_malloc(length + 1);
  if (!newString)
    return NULL;
  strcpy(newString, string);
  return newString;
}

char *wsky_strndup(const char *string, size_t maximum) {
  size_t length = strlen(string);
  if (maximum < length)
    length = maximum;
  char *newString = wsky_malloc(length + 1);
  if (!newString)
    return NULL;
  strncpy(newString, string, length);
  newString[length] = '\0';
  return newString;
}
