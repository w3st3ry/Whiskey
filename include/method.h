#ifndef METHOD_H_
# define METHOD_H_

# include "return_value.h"

typedef struct wsky_MethodDef_s wsky_MethodDef;



typedef wsky_ReturnValue (*wsky_VariadicMethod)(wsky_Object *o,
                                                unsigned parameterCount,
                                                wsky_Value *parameters);

typedef wsky_ReturnValue (*wsky_Method0)(wsky_Object *o);
typedef wsky_ReturnValue (*wsky_Method1)(wsky_Object *o,
                                         wsky_Value *a);
typedef wsky_ReturnValue (*wsky_Method2)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b);
typedef wsky_ReturnValue (*wsky_Method3)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c);
typedef wsky_ReturnValue (*wsky_Method4)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d);
typedef wsky_ReturnValue (*wsky_Method5)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e);
typedef wsky_ReturnValue (*wsky_Method6)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e,
                                         wsky_Value *f);
typedef wsky_ReturnValue (*wsky_Method7)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e,
                                         wsky_Value *f,
                                         wsky_Value *g);
typedef wsky_ReturnValue (*wsky_Method8)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e,
                                         wsky_Value *f,
                                         wsky_Value *g,
                                         wsky_Value *h);

struct wsky_MethodDef_s {
  const char *name;

  /** -1 if variable parameter count */
  int parameterCount;

  void *function;
};

wsky_ReturnValue wsky_MethodDef_call(const wsky_MethodDef *method,
                                     wsky_Object *object,
                                     unsigned parameterCount,
                                     wsky_Value *parameters);
void wsky_MethodDef_printDebug(const wsky_MethodDef *this);



typedef struct wsky_MethodList_s wsky_MethodList;

struct wsky_MethodList_s {
  wsky_MethodDef *methods;
  unsigned count;
  unsigned available;
};

void wsky_MethodList_init(wsky_MethodList *this, unsigned maxCount);
void wsky_MethodList_free(wsky_MethodList *this);
void wsky_MethodList_add(wsky_MethodList *this, wsky_MethodDef *method);
void wsky_MethodList_printDebug(const wsky_MethodList *this);

#endif /* !METHOD_H_ */
