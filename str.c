#include "str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

degat_Class degat_String_CLASS = {
  .super = &degat_Object_CLASS,
  .name = "String",
  .constructor = &degat_String_construct,
  .destructor = &degat_String_destroy,
  .objectSize = sizeof(degat_String)
};



void degat_String_initClass(void) {
  degat_MethodList *ml = (degat_MethodList *) &degat_String_CLASS.methods;
  degat_MethodList_init(ml, 10);

#define ADD(name_, paramCount_) \
  degat_MethodList_addNew(ml, #name_, paramCount_, \
  (void *) &degat_String_ ## name_)

  ADD(getLength, 0);
  ADD(startsWith, 1);
  ADD(indexOf, 1);
  ADD(contains, 1);

#undef ADD
}

void degat_String_freeClass(void) {
  degat_MethodList *ml = (degat_MethodList *) &degat_String_CLASS.methods;
  degat_MethodList_free(ml);
}



degat_String *degat_String_new(const char *cString) {
  degat_ReturnValue r = degat_Object_new(&degat_String_CLASS,
					 0, NULL);
  if (r.exception)
    return NULL;
  degat_String *string = (degat_String *) r.v.v.objectValue;
  string->string = strdup(cString);
  return string;
}

degat_Exception *degat_String_construct(degat_Object *object,
					unsigned paramCount,
					degat_Value *params) {
  (void) paramCount;
  (void) params;

  degat_String *this = (degat_String *) object;
  this->string = NULL;
  return NULL;
}

void degat_String_destroy(degat_Object *object) {
  degat_String *this = (degat_String *) object;
  if (this->string)
    free(this->string);
}



bool degat_isString(const degat_Value value) {
  if (value.type != degat_Type_OBJECT)
    return false;
  if (degat_Value_isNull(value))
    return false;
  return value.v.objectValue->class == &degat_String_CLASS;
}

degat_String *degat_Value_toString(degat_Value value) {
  if (!degat_isString(value))
    {
      fprintf(stderr, "degat_Value_toString(): error");
      abort();
    }
  return (degat_String *) value.v.objectValue;
}



degat_ReturnValue degat_String_getLength(degat_String *this) {
  degat_RETURN_INT((int32_t) strlen(this->string));
}

degat_ReturnValue degat_String_equals(degat_String *this,
				      degat_Value otherV) {
  if (!degat_isString(otherV))
    degat_RETURN_FALSE;
  degat_String *other = degat_Value_toString(otherV);
  degat_RETURN_BOOL(strcmp(this->string, other->string));
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

degat_ReturnValue degat_String_startsWith(degat_String *this,
					  degat_Value otherV) {
  if (!degat_isString(otherV))
    degat_RETURN_FALSE;
  degat_String *prefix = degat_Value_toString(otherV);
  degat_RETURN_BOOL(startsWith(this->string, prefix->string));
}

degat_ReturnValue degat_String_indexOf(degat_String *this,
				       degat_Value otherV) {
  if (!degat_isString(otherV))
    degat_RETURN_FALSE;
  degat_String *other = degat_Value_toString(otherV);
  degat_RETURN_INT(indexOf(this->string, other->string));
}

degat_ReturnValue degat_String_contains(degat_String *this,
					degat_Value otherV) {
  if (!degat_isString(otherV))
    degat_RETURN_FALSE;
  degat_String *other = degat_Value_toString(otherV);
  degat_RETURN_BOOL(indexOf(this->string, other->string) != -1);
}

void degat_String_print(const degat_String *this) {
  printf("%s", this->string);
}
