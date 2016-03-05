#ifndef STRUCTURE_H
# define STRUCTURE_H

# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Structure Structure
 * @{
 */


extern const wsky_ClassDef wsky_Structure_CLASS_DEF;

extern wsky_Class *wsky_Structure_CLASS;


/** A Structure */
typedef struct wsky_Structure_s {
  wsky_OBJECT_HEAD

  wsky_Dict members;
} wsky_Structure;

/**
 * Creates a new Structure.
 */
wsky_Structure *wsky_Structure_new(void);

/**
 * Sets a structure member.
 * Can raise an exception.
 * Returns the given value on success.
 */
wsky_ReturnValue wsky_Structure_set(wsky_Structure *self,
                                    const char *name,
                                    const wsky_Value *value);

/**
 * Gets a structure member.
 * Raises an exception if the member does not exists.
 */
wsky_ReturnValue wsky_Structure_get(wsky_Structure *self, const char *name);

/**
 * @}
 * @}
 */

#endif /* STRUCTURE_H */
