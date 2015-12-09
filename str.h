#ifndef STR_H_
# define STR_H_

# include "object.h"

extern degat_Class degat_String_CLASS;

void degat_String_initClass(void);
void degat_String_freeClass(void);



struct degat_String_s;
typedef struct degat_String_s degat_String;

struct degat_String_s
{
  degat_OBJECT_HEAD

  char *string;
};



degat_String *degat_String_new(const char *cString);

degat_Exception *degat_String_construct(degat_Object *this,
					unsigned paramCount,
					degat_Value *params);
void degat_String_destroy(degat_Object *this);



bool degat_isString(const degat_Value value);
degat_String *degat_Value_toString(degat_Value value);



degat_ReturnValue degat_String_getLength(degat_String *this);
degat_ReturnValue degat_String_equals(degat_String *this,
				      degat_Value other);
degat_ReturnValue degat_String_startsWith(degat_String *this,
					  degat_Value otherV);
degat_ReturnValue degat_String_indexOf(degat_String *this,
				       degat_Value otherV);
degat_ReturnValue degat_String_contains(degat_String *this,
					degat_Value otherV);

void degat_String_print(degat_String *this);

#endif /* !STR_H_ */
