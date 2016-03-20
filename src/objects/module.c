#include "objects/module.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "gc.h"
#include "string_utils.h"
#include "../return_value_private.h"

#include "objects/str.h"
#include "objects/class.h"


typedef wsky_Module Module;
typedef wsky_ModuleList ModuleList;
typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_ProgramFile ProgramFile;
typedef wsky_Value Value;
typedef wsky_Dict Dict;


static ModuleList *modules = NULL;



static ModuleList *ModuleList_new(Module *module, ModuleList *next) {
  ModuleList *this = wsky_safeMalloc(sizeof(ModuleList));
  this->module = module;
  this->next = next;
  return this;
}

static void ModuleList_add(ModuleList **list, Module *newModule) {
  *list = ModuleList_new(newModule, *list);
}

static void ModuleList_delete(ModuleList *list) {
  if (!list)
    return;
  ModuleList_delete(list->next);
  wsky_free(list);
}



static ReturnValue toString(Module *self);

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);



#define GET_NAME(function, name)                                \
  {#name, 0, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,    \
      (wsky_Method0)&function}

#define GET(name) GET_NAME(name, name)


static wsky_MethodDef methods[] = {
  GET(toString),
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_Module_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Module",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Module),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

Class *wsky_Module_CLASS;


Module *wsky_Module_new(const char *name,
                        bool builtin,
                        ProgramFile *file) {
  if (file == NULL)
    assert(builtin || strcmp(name, "__main__") == 0);

  ReturnValue r = wsky_Object_new(wsky_Module_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Module *module = (Module *)r.v.v.objectValue;

  module->name = wsky_strdup(name);
  wsky_Dict_init(&module->members);
  module->builtin = builtin;
  if (!file)
    file = wsky_ProgramFile_getUnknown();
  module->file = file;

  if (strcmp(name, "__main__") != 0)
    ModuleList_add(&modules, module);

  return module;
}

wsky_Module *wsky_Module_newMain(void) {
  return wsky_Module_new("__main__", false, NULL);
}


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  // TODO
  (void)object;
  (void)paramCount;
  (void)params;
  RETURN_NULL;
}


static void freeMember(const char *name, void *valuePointer) {
  (void)name;
  wsky_free(valuePointer);
}

static ReturnValue destroy(Object *object) {
  Module *module = (Module *)object;
  free(module->name);
  wsky_Dict_apply(&module->members, freeMember);
  wsky_Dict_free(&module->members);
  RETURN_NULL;
}


static void visitMember(const char *name, void *valuePointer) {
  (void)name;
  wsky_GC_visitValue(*(Value*)valuePointer);
}

static void acceptGC(Object *object) {
  Module *module = (Module *)object;
  wsky_GC_visitObject(module->file);
  wsky_Dict_apply(&module->members, visitMember);
}



void wsky_Module_addValue(Module *module,
                          const char *name,
                          Value value) {
  assert(!wsky_Dict_contains(&module->members, name));
  Value *valuePointer = wsky_Value_new(value);
  if (!valuePointer)
    abort();
  wsky_Dict_set(&module->members, name, valuePointer);
}

void wsky_Module_addObject(Module *module,
                           const char *name,
                           Object *object) {
  wsky_Module_addValue(module, name, wsky_Value_fromObject(object));
}

void wsky_Module_addFunction(Module *module,
                             const char *name,
                             int parameterCount,
                             wsky_Method0 function) {
  wsky_MethodDef def = {
    name, parameterCount, 0, function,
  };
  wsky_Function *f = wsky_Function_newFromC(name, &def);
  wsky_Module_addObject(module, name, (Object *)f);
}



ModuleList *wsky_Module_getModules(void) {
  return modules;
}

void wsky_Module_deleteModules(void) {
  ModuleList_delete(modules);
}


static ReturnValue toString(Module *self) {
  (void) self;
  char buffer[64];
  snprintf(buffer, 63, "<Module %s>", self->name);
  RETURN_C_STRING(buffer);
}
