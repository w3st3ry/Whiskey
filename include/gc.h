#ifndef WSKY_GC_H_
# define WSKY_GC_H_

# include "value.h"

/** Don't call this function directly. */
void wsky_GC_initImpl(void *stackStart);

/**
 * Initializes the garbage collector.
 *
 * Called by wsky_start().
 */
static inline void wsky_GC_init(void) {
  int n;
  wsky_GC_initImpl(&n);
}

void wsky_GC_unmarkAll(void);

void wsky_GC_collect(void);

void wsky_GC_deleteAll(void);

void wsky_GC_autoCollect(void);

/**
 * Requests a garbage collection.
 *
 * If the stress mode is disabled, the garbage collection
 * is performed only if needed.
 * If the stress mode is enabled, the garbage collection
 * is performed in anyway.
 */
void wsky_GC_requestCollection(void);

void wsky_GC_visitObject(void *object);

void wsky_GC_visitValue(wsky_Value v);


/**
 * Sets the stress mode. This is for debugging purposes.
 *
 * If the stress mode is enabled, the garbage collection runs
 * at every object creation.
 *
 * The stress mode can be set even if wsky_start() has not been called.
 */
void wsky_GC_setStressed(bool stressed);

/**
 * Returns true if the stress mode is enabled.
 */
bool wsky_GC_isStressed(void);


#endif /* !WSKY_GC_H_ */
