#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include "object.h"


extern degat_Class degat_Exception_CLASS;

void degat_String_initClass(void);
void degat_String_freeClass(void);



struct degat_Exception_s
{
  degat_OBJECT_HEAD

  char *message;
  struct degat_Exception_s *cause;
};



degat_Exception *degat_Exception_new(const char *message,
				     degat_Exception *cause);
degat_Exception *degat_Exception_construct(degat_Object *object,
					   unsigned paramCount,
					   degat_Value *params);

void degat_Exception_destroy(degat_Object *this);



void degat_Exception_print(const degat_Exception *this);

#endif /* !EXCEPTION_H_ */
