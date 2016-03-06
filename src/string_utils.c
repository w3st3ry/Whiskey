#include "string_utils.h"

#include <stdio.h>
#include <string.h>
#include "gc.h"

static int wsky_vsnprintf(char *str, size_t size, const char *format,
                          va_list ap) {
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wformat-nonliteral"
#elif __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

  int out = vsnprintf(str, size, format, ap);

#ifdef __GNUC__
# pragma GCC diagnostic pop
#elif __clang__
# pragma clang diagnostic pop
#endif

  return out;
}

char *wsky_asprintf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  va_list copy;
  va_copy(copy, ap);
  int out = wsky_vsnprintf(NULL, 0, fmt, copy);
  va_end(copy);
  if (out < 0)
    return (NULL);
  size_t length = out;
  void *ptr = wsky_safeMalloc(length + 1);
  if (!ptr)
    return NULL;
  out = vsprintf(ptr, fmt, ap);
  if (out < 0) {
    wsky_free(ptr);
    return NULL;
  }
  va_end(ap);
  return ptr;
}

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
