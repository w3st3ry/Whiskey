#ifndef WSKY_GC_H_
# define WSKY_GC_H_

# include <stddef.h>
# include "value.h"
# include "objects/scope.h"



# define wsky_MALLOC(size) malloc(size)
# define wsky_MALLOC_ATOMIC(size) malloc(size)
# define wsky_REALLOC(data, size) realloc(data, size)
# define wsky_FREE(data) free(data)

# define wsky_STRDUP(string) strdup(string)
# define wsky_STRNDUP(string, length) strndup(string, length)


void wsky_GC_unmarkAll(void);
void wsky_GC_collect(void);

void wsky_GC_register(wsky_Object *object);

void wsky_GC__visit(void *object);
#define wsky_GC_VISIT(object) wsky_GC__visit(object)

void wsky_GC__visitValue(wsky_Value v);
#define wsky_GC_VISIT_VALUE(object) wsky_GC__visitValue(object)

void wsky_GC_visitBuiltins(void);


#endif /* !WSKY_GC_H_ */
