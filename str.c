#include "str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

wsky_Class wsky_String_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "String",
  .constructor = &wsky_String_construct,
  .destructor = &wsky_String_destroy,
  .objectSize = sizeof(wsky_String)
};



void wsky_String_initClass(void) {
  wsky_MethodList *ml = (wsky_MethodList *) &wsky_String_CLASS.methods;
  wsky_MethodList_init(ml, 10);

#define ADD(name_, paramCount_) \
  wsky_MethodList_addNew(ml, #name_, paramCount_, \
  (void *) &wsky_String_ ## name_)

  ADD(getLength, 0);
  ADD(startsWith, 1);
  ADD(indexOf, 1);
  ADD(contains, 1);

#undef ADD
}

void wsky_String_freeClass(void) {
  wsky_MethodList *ml = (wsky_MethodList *) &wsky_String_CLASS.methods;
  wsky_MethodList_free(ml);
}



wsky_String *wsky_String_new(const char *cString) {
  wsky_ReturnValue r = wsky_Object_new(&wsky_String_CLASS,
					 0, NULL);
  if (r.exception)
    return NULL;
  wsky_String *string = (wsky_String *) r.v.v.objectValue;
  string->string = strdup(cString);
  return string;
}

wsky_Exception *wsky_String_construct(wsky_Object *object,
					unsigned paramCount,
					wsky_Value *params) {
  (void) paramCount;
  (void) params;

  wsky_String *this = (wsky_String *) object;
  this->string = NULL;
  return NULL;
}

void wsky_String_destroy(wsky_Object *object) {
  wsky_String *this = (wsky_String *) object;
  if (this->string)
    free(this->string);
}



bool wsky_isString(const wsky_Value value) {
  if (value.type != wsky_Type_OBJECT)
    return false;
  if (wsky_Value_isNull(value))
    return false;
  return value.v.objectValue->class == &wsky_String_CLASS;
}

wsky_String *wsky_Value_toString(wsky_Value value) {
  if (!wsky_isString(value)) {
    fprintf(stderr, "wsky_Value_toString(): error");
    abort();
  }
  return (wsky_String *) value.v.objectValue;
}



wsky_ReturnValue wsky_String_getLength(wsky_String *this) {
  wsky_RETURN_INT((int32_t) strlen(this->string));
}

wsky_ReturnValue wsky_String_equals(wsky_String *this,
				      wsky_Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  wsky_String *other = wsky_Value_toString(otherV);
  wsky_RETURN_BOOL(strcmp(this->string, other->string));
}

static bool startsWith(const char *a, const char *prefix) {
  while (*a == *prefix) {
    if (!*a)
      return true;
    a++;
    prefix++;
  }
  return !prefix;
}

static int64_t indexOf(const char *a, const char *target) {
  int index = 0;
  while (*a) {
    if (startsWith(a, target)) {
      return index;
    }
    index++;
    a++;
  }
  return -1;
}

wsky_ReturnValue wsky_String_startsWith(wsky_String *this,
					  wsky_Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  wsky_String *prefix = wsky_Value_toString(otherV);
  wsky_RETURN_BOOL(startsWith(this->string, prefix->string));
}

wsky_ReturnValue wsky_String_indexOf(wsky_String *this,
				       wsky_Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  wsky_String *other = wsky_Value_toString(otherV);
  wsky_RETURN_INT(indexOf(this->string, other->string));
}

wsky_ReturnValue wsky_String_contains(wsky_String *this,
					wsky_Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  wsky_String *other = wsky_Value_toString(otherV);
  wsky_RETURN_BOOL(indexOf(this->string, other->string) != -1);
}

void wsky_String_print(const wsky_String *this) {
  printf("%s", this->string);
}
