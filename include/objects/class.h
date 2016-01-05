#ifndef CLASS_H
# define CLASS_H

# include "object.h"
# include "method.h"
# include "dict.h"


extern const wsky_ClassDef wsky_Class_CLASS_DEF;

extern wsky_Class *wsky_Class_CLASS;


/** A Whiskey class object */
struct wsky_Class_s {
  wsky_OBJECT_HEAD

  /** The name of the class */
  char *name;

  /** The superclass */
  struct wsky_Class_s *super;

  /** The methods and the accessors */
  wsky_Dict *methods;

  /** The constructor */
  wsky_MethodObject *constructor;

  /** The destructor or NULL */
  wsky_Method0 destructor;

  /** The object size in bytes */
  size_t objectSize;
};


wsky_Class *wsky_Class_new(const wsky_ClassDef *def, wsky_Class *super);

void wsky_Class_acceptGC(wsky_Object *object);
void wsky_Class_destroyObject(wsky_Object *object);


#endif /* CLASS_H */
