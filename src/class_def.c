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

#include "objects/program_file.h"

#include "objects/exception.h"
#include "objects/syntax_error_ex.h"
#include "objects/type_error.h"
#include "objects/value_error.h"
#include "objects/not_implemented_error.h"
#include "objects/attribute_error.h"

#include "gc.h"


typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_MethodDef MethodDef;


typedef struct {
  const ClassDef *def;
  Class **classPointer;
} ClassInfo;

static ClassInfo CLASSES[] = {
#define C(name) {&wsky_ ## name ## _CLASS_DEF, &wsky_ ## name ## _CLASS}

  C(Class),
  C(Object),
  C(MethodObject),
  C(InstanceMethod),
  C(Scope),
  C(Function),

  C(Null),
  C(Boolean),
  C(Integer),
  C(Float),
  C(String),

  C(ProgramFile),

  C(Exception),
  C(SyntaxErrorEx),
  C(TypeError),
  C(ValueError),
  C(NotImplementedError),
  C(AttributeError),

  {0, 0},
};

static Class* getClass(const ClassDef *def) {
  ClassInfo *classInfo = CLASSES;
  while (classInfo->def) {
    if (classInfo->def == def) {
      return *classInfo->classPointer;
    }
    classInfo++;
  }
  fprintf(stderr, "Class definition not found: %s\n", def->name);
  abort();
}

static Class* getSuperClass(const ClassDef *def) {
  if (def == &wsky_Class_CLASS_DEF || def == &wsky_Object_CLASS_DEF) {
    return NULL;
  }

  if (!def->super) {
    fprintf(stderr, "%s has no superclass\n", def->name);
    return NULL;
  }
  return getClass(def->super);
}

static void initClass(ClassInfo *info) {
  const ClassDef *def = info->def;
  Class **classPointer = info->classPointer;
  *classPointer = wsky_Class_new(def, getSuperClass(def));

  if (def == &wsky_Object_CLASS_DEF) {
    wsky_Object_CLASS->class = wsky_Class_CLASS;
    wsky_Class_CLASS->class = wsky_Class_CLASS;
    wsky_Class_CLASS->super = wsky_Object_CLASS;
  }
}

void wsky_GC_visitBuiltins(void) {
  ClassInfo *classInfo = CLASSES;
  while (classInfo->def) {
    wsky_GC_VISIT(*classInfo->classPointer);
    classInfo++;
  }
}

void wsky_start(void) {
  ClassInfo *classInfo = CLASSES;
  while (classInfo->def) {
    *classInfo->classPointer = NULL;
    classInfo++;
  }

  classInfo = CLASSES;
  while (classInfo->def) {
    initClass(classInfo);
    classInfo++;
  }

  wsky_Class_initMethods(wsky_Class_CLASS, &wsky_Class_CLASS_DEF);
  wsky_Class_initMethods(wsky_Object_CLASS, &wsky_Object_CLASS_DEF);
  wsky_Class_initMethods(wsky_MethodObject_CLASS,
                         &wsky_MethodObject_CLASS_DEF);
}

void wsky_stop(void) {
  wsky_GC_unmarkAll();
  wsky_GC_collect();
}
