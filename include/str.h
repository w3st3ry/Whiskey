#ifndef STR_H_
# define STR_H_

# include "object.h"

extern wsky_Class wsky_String_CLASS;

void wsky_String_initClass(void);
void wsky_String_freeClass(void);



struct wsky_String_s {
  wsky_OBJECT_HEAD

  char *string;
};



wsky_String *wsky_String_new(const char *cString);

bool wsky_isString(const wsky_Value value);

wsky_ReturnValue wsky_String_getLength(wsky_String *this);
wsky_ReturnValue wsky_String_equals(wsky_String *this,
                                      wsky_Value other);
wsky_ReturnValue wsky_String_startsWith(wsky_String *this,
                                          wsky_Value otherV);
wsky_ReturnValue wsky_String_indexOf(wsky_String *this,
                                       wsky_Value otherV);
wsky_ReturnValue wsky_String_contains(wsky_String *this,
                                        wsky_Value otherV);

void wsky_String_print(const wsky_String *this);



char *wsky_String_escapeCString(const char *source);

#endif /* !STR_H_ */
