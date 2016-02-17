#include "objects/module.h"
#include "gc.h"

typedef wsky_Module Module;
typedef wsky_ModuleList ModuleList;
typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_ProgramFile ProgramFile;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
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



static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);


static wsky_MethodDef methods[] = {
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


Module *wsky_Module_new(const char *name, Dict *members,
                        bool builtin,
                        ProgramFile *file) {
  ReturnValue r = wsky_Object_new(wsky_Module_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Module *module = (Module *)r.v.v.objectValue;

  module->name = wsky_strdup(name);
  module->members = *members;
  module->builtin = builtin;
  if (!file)
    file = wsky_ProgramFile_getUnknown();
  module->file = file;

  ModuleList_add(&modules, module);

  return module;
}

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  (void)object;
  (void)paramCount;
  (void)params;
  wsky_RETURN_NULL;
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
  wsky_RETURN_NULL;
}


static void visitMember(const char *name, void *valuePointer) {
  (void)name;
  wsky_GC_VISIT_VALUE(*(Value*)valuePointer);
}

static void acceptGC(Object *object) {
  Module *module = (Module *)object;
  wsky_GC_VISIT(module->file);
  wsky_Dict_apply(&module->members, visitMember);
}



ModuleList *wsky_Module_getModules(void) {
  return modules;
}

void wsky_Module_deleteModules(void) {
  ModuleList_delete(modules);
}
