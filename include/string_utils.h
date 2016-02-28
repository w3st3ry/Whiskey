#ifndef STRING_UTILS_H_
# define STRING_UTILS_H_

# include <stddef.h>

/** Like strdup() */
char *wsky_strdup(const char *string);

/** Like strndup() */
char *wsky_strndup(const char *string, size_t maximum);

#endif /* !STRING_UTILS_H_ */
