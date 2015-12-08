#ifndef METHOD_H_
# define METHOD_H_

#include "return_value.h"

struct degat_Method_s;
typedef struct degat_Method_s degat_Method;

typedef degat_ReturnValue (*degat_VariadicMethod)(degat_Object *o,
						  unsigned parameterCount,
						  degat_Value *parameters);
typedef degat_ReturnValue (*degat_Method0)(degat_Object *o);
typedef degat_ReturnValue (*degat_Method1)(degat_Object *o,
					   degat_Value *a);
typedef degat_ReturnValue (*degat_Method2)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b);
typedef degat_ReturnValue (*degat_Method3)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b,
					   degat_Value *c);
typedef degat_ReturnValue (*degat_Method4)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b,
					   degat_Value *c,
					   degat_Value *d);
typedef degat_ReturnValue (*degat_Method5)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b,
					   degat_Value *c,
					   degat_Value *d,
					   degat_Value *e);
typedef degat_ReturnValue (*degat_Method6)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b,
					   degat_Value *c,
					   degat_Value *d,
					   degat_Value *e,
					   degat_Value *f);
typedef degat_ReturnValue (*degat_Method7)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b,
					   degat_Value *c,
					   degat_Value *d,
					   degat_Value *e,
					   degat_Value *f,
					   degat_Value *g);
typedef degat_ReturnValue (*degat_Method8)(degat_Object *o,
					   degat_Value *a,
					   degat_Value *b,
					   degat_Value *c,
					   degat_Value *d,
					   degat_Value *e,
					   degat_Value *f,
					   degat_Value *g,
					   degat_Value *h);

struct degat_Method_s {
  const char *name;

  /* -1 if variable parameter count */
  int parameterCount;

  void *function;
};

degat_Method degat_Method_create(const char *name,
				 int parameterCount,
				 void *function);
degat_ReturnValue degat_Method_call(const degat_Method *method,
				    degat_Object *object,
				    unsigned parameterCount,
				    degat_Value *parameters);
void degat_Method_printDebug(const degat_Method *this);



struct degat_MethodList_s;
typedef struct degat_MethodList_s degat_MethodList;

struct degat_MethodList_s {
  degat_Method *methods;
  int count;
  int available;
};

void degat_MethodList_init(degat_MethodList *this, int maxCount);
void degat_MethodList_free(degat_MethodList *this);
void degat_MethodList_add(degat_MethodList *this, degat_Method *method);
void degat_MethodList_addNew(degat_MethodList *this,
			     const char *name,
			     int parameterCount,
			     void *function);
void degat_MethodList_printDebug(const degat_MethodList *this);

#endif /* !METHOD_H_ */
