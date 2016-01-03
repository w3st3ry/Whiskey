#include "class_def.h"

#include <stdlib.h>
#include <string.h>
#include "objects/class.h"
#include "objects/null.h"
#include "objects/boolean.h"
#include "objects/integer.h"
#include "objects/float.h"
#include "objects/str.h"
#include "objects/function.h"
#include "objects/instance_method.h"
#include "gc.h"


typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_MethodDef MethodDef;


typedef struct {
  const ClassDef *def;
  Class **classPointer;
} ClassInfo;


static void initClass(ClassInfo *info) {
  const ClassDef *def = info->def;
  Class **classPointer = info->classPointer;
  *classPointer = wsky_Class_new(def);
}

static ClassInfo CLASSES[] = {
#define C(name) {&wsky_ ## name ## _CLASS_DEF, &wsky_ ## name ## _CLASS}

  C(Class),
  C(Object),
  C(InstanceMethod),
  C(Scope),
  C(Function),

  C(Null),
  C(Boolean),
  C(Integer),
  C(Float),
  C(String),

  {0, 0},
};

void wsky_start(void) {
  ClassInfo *classInfo = CLASSES;
  while (classInfo->def) {
    initClass(classInfo);
    classInfo++;
  }
}

void wsky_stop(void) {
  wsky_GC_unmarkAll();
  wsky_GC_collect();
}
