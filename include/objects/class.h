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

  /** True if the class is written in C */
  bool native;

  /** The superclass */
  struct wsky_Class_s *super;

  /** The methods and the getters */
  wsky_Dict *methods;

  /** The setters */
  wsky_Dict *setters;

  /** The constructor */
  wsky_Method *constructor;

  /** The destructor or NULL */
  wsky_Method0 destructor;

  /** The object size in bytes */
  size_t objectSize;

  /** The accept function, used by the garbage collector */
  wsky_GCAcceptFunction gcAcceptFunction;
};


wsky_Class *wsky_Class_new(const char *name, wsky_Class *super);
wsky_Class *wsky_Class_newFromC(const wsky_ClassDef *def, wsky_Class *super);
void wsky_Class_initMethods(wsky_Class *class, const wsky_ClassDef *def);

static inline bool wsky_isClass(wsky_Value value) {
  return wsky_getClass(value) == wsky_Class_CLASS;
}

/** Calls the constructor */
wsky_ReturnValue wsky_Class_construct(wsky_Class *class,
                                      unsigned parameterCount,
                                      wsky_Value *parameters);

void wsky_Class_acceptGC(wsky_Object *object);
void wsky_Class_destroyObject(wsky_Object *object);



wsky_ReturnValue wsky_Class_getField(wsky_Class *class, wsky_Object *self,
                                     const char *name);

wsky_ReturnValue wsky_Class_callGetter(wsky_Class *class, wsky_Object *self,
                                       wsky_Method *method, const char *name);

wsky_ReturnValue wsky_Class_get(wsky_Class *class, wsky_Object *self,
                                const char *attribute);



wsky_ReturnValue wsky_Class_setField(wsky_Class *class, wsky_Object *self,
                                     const char *name,
                                     const wsky_Value *value);

wsky_ReturnValue wsky_Class_callSetter(wsky_Class *class, wsky_Object *self,
                                       wsky_Method *method, const char *name,
                                       const wsky_Value *value);

wsky_ReturnValue wsky_Class_set(wsky_Class *class, wsky_Object *self,
                                const char *attribute,
                                const wsky_Value *value);



/** Finds a method or a getter in this class, not in the superclasses */
wsky_Method *wsky_Class_findLocalMethod(wsky_Class *class, const char *name);

/** Finds a method or a getter in this class and in the superclasses */
wsky_Method *wsky_Class_findMethodOrGetter(wsky_Class *class,
                                           const char *name);

/** Finds a setter in this class and in the superclasses */
wsky_Method *wsky_Class_findLocalSetter(wsky_Class *class, const char *name);

/** Finds a setter in this class and in the superclasses */
wsky_Method *wsky_Class_findSetter(wsky_Class *class, const char *name);


#endif /* CLASS_H */
