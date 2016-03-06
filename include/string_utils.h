#ifndef STRING_UTILS_H_
# define STRING_UTILS_H_

# include <stddef.h>
# include <stdarg.h>

/**
 * Like asprintf(), except that it returns the pointer instead of a parameter.
 * Remember to free() the pointer returned by this function.
*/
char *wsky_asprintf(const char *fmt, ...);

/** Like vsnprintf() */
int wsky_vsnprintf(char *, size_t, const char *, va_list);

/** Like strdup() */
char *wsky_strdup(const char *string);

/** Like strndup() */
char *wsky_strndup(const char *string, size_t maximum);

#endif /* !STRING_UTILS_H_ */
