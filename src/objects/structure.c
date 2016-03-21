#include "../whiskey_private.h"


static ReturnValue toString(Structure *self);

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);


#define GET_NAME(function, name)                                \
  {#name, 0, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,    \
      (wsky_Method0)&function}

#define GET(name) GET_NAME(name, name)


static MethodDef methods[] = {
  GET(toString),
  {0, 0, 0, 0},
};

const ClassDef wsky_Structure_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Structure",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Structure),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

Class *wsky_Structure_CLASS;


static ReturnValue construct(Object *object,
                             unsigned parameterCount,
                             const Value *parameters) {
  (void)parameterCount;
  (void)parameters;
  Structure *self = (Structure *)object;
  wsky_Dict_init(&self->members);
  RETURN_NULL;
}


static void freeMember(const char *name, void *valuePointer) {
  (void)name;
  wsky_free(valuePointer);
}

static ReturnValue destroy(Object *object) {
  Structure *self = (Structure *)object;
  wsky_Dict_apply(&self->members, freeMember);
  wsky_Dict_free(&self->members);
  RETURN_NULL;
}


static void visitMember(const char *name, void *valuePointer) {
  (void)name;
  wsky_GC_visitValue(*(Value*)valuePointer);
}

static void acceptGC(Object *object) {
  Structure *self = (Structure *)object;
  wsky_Dict_apply(&self->members, visitMember);
}

static ReturnValue toString(Structure *self) {
  (void)self;
  RETURN_C_STRING("<Structure>");
}


ReturnValue wsky_Structure_set(Structure *self,
                               const char *name,
                               Value value) {
  Value *newValue = wsky_safeMalloc(sizeof(Value));
  *newValue = value;
  wsky_Dict_set(&self->members, name, newValue);
  RETURN_VALUE(value);
}

ReturnValue wsky_Structure_get(Structure *self, const char *attribute) {
  if (!wsky_Dict_contains(&self->members, attribute))
    return wsky_AttributeError_raiseNoAttr(wsky_Structure_CLASS->name,
                                           attribute);

  Value *value = (Value *)wsky_Dict_get(&self->members, attribute);
  RETURN_VALUE(*value);
}
