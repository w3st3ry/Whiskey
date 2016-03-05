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
#include "objects/module.h"
#include "objects/structure.h"

#include "objects/program_file.h"

#include "objects/attribute_error.h"
#include "objects/exception.h"
#include "objects/name_error.h"
#include "objects/not_implemented_error.h"
#include "objects/parameter_error.h"
#include "objects/syntax_error_ex.h"
#include "objects/type_error.h"
#include "objects/value_error.h"

#include "gc.h"


/** This structure holds informations about a class */
typedef struct {

  /** A pointer to the definition of the class */
  const wsky_ClassDef *def;
  wsky_Class **classPointer;
} wsky_BuiltinClassInfo;


typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_BuiltinClassInfo ClassInfo;
typedef wsky_MethodDef MethodDef;


static const ClassInfo BUILTIN_CLASSES[] = {
#define C(name) {&wsky_ ## name ## _CLASS_DEF, &wsky_ ## name ## _CLASS}

  C(Class),
  C(Object),
  C(Function),
  C(Method),
  C(InstanceMethod),
  C(Scope),
  C(Structure),
  C(Module),

  C(Null),
  C(Boolean),
  C(Integer),
  C(Float),
  C(String),

  C(ProgramFile),

  C(Exception),

  C(AttributeError),
  C(NameError),
  C(NotImplementedError),
  C(ParameterError),
  C(SyntaxErrorEx),
  C(TypeError),
  C(ValueError),

  {0, 0},
};


static wsky_ClassArray builtinsClassArray = {NULL, 0};


static size_t getBuiltinClassesCount(void) {
  size_t count = 0;
  for (const ClassInfo *info = BUILTIN_CLASSES; info->def; info++)
    count++;
  return count;
}

static void initBuiltinsClassArray(void) {
  size_t count = getBuiltinClassesCount();
  builtinsClassArray.count = count;
  builtinsClassArray.classes = malloc(sizeof(Class *) * count);
  if (!builtinsClassArray.classes)
    abort();

  for (size_t i = 0; i < count; i++) {
    const ClassInfo *info = BUILTIN_CLASSES + i;
    builtinsClassArray.classes[i] = *info->classPointer;
  }
}

const wsky_ClassArray *wsky_getBuiltinClasses(void) {
  return &builtinsClassArray;
}


static Class* getClass(const ClassDef *def) {
  const ClassInfo *classInfo = BUILTIN_CLASSES;
  while (classInfo->def) {
    if (classInfo->def == def)
      return *classInfo->classPointer;

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

static void initClass(const ClassInfo *info) {
  const ClassDef *def = info->def;
  Class **classPointer = info->classPointer;
  *classPointer = wsky_Class_newFromC(def, getSuperClass(def));

  if (def == &wsky_Object_CLASS_DEF) {
    wsky_Object_CLASS->class = wsky_Class_CLASS;
    wsky_Class_CLASS->class = wsky_Class_CLASS;
    wsky_Class_CLASS->super = wsky_Object_CLASS;
  }
}


void wsky_initBuiltinClasses(void) {
  const ClassInfo *classInfo = BUILTIN_CLASSES;
  while (classInfo->def) {
    *classInfo->classPointer = NULL;
    classInfo++;
  }

  classInfo = BUILTIN_CLASSES;
  while (classInfo->def) {
    initClass(classInfo);
    classInfo++;
  }

  wsky_Class_initMethods(wsky_Class_CLASS, &wsky_Class_CLASS_DEF);
  wsky_Class_initMethods(wsky_Object_CLASS, &wsky_Object_CLASS_DEF);
  wsky_Class_initMethods(wsky_Function_CLASS, &wsky_Function_CLASS_DEF);
  wsky_Class_initMethods(wsky_Method_CLASS, &wsky_Method_CLASS_DEF);

  initBuiltinsClassArray();
}

void wsky_freeBuiltinClasses(void) {

  free(builtinsClassArray.classes);
}
