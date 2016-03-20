#ifndef WSKY_GC_H_
# define WSKY_GC_H_

# include "value.h"

void wsky_GC_initImpl(void *stackStart);

static inline void wsky_GC_init(void) {
  int n;
  wsky_GC_initImpl(&n);
}

void wsky_GC_unmarkAll(void);

void wsky_GC_collect(void);

void wsky_GC_deleteAll(void);

void wsky_GC_autoCollect(void);

void wsky_GC_visitObject(void *object);

void wsky_GC_visitValue(wsky_Value v);


#endif /* !WSKY_GC_H_ */
