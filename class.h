#ifndef CLASS_H_
# define CLASS_H_

#include "ctor.h"
#include "method.h"
#include <stdint.h>

struct degat_Class_s;
typedef struct degat_Class_s degat_Class;

struct degat_Class_s
{
  const struct degat_Class_s *super;
  const char *name;
  degat_Constructor constructor;
  degat_Destructor destructor;
  uint32_t objectSize;
  degat_MethodList methods;
};


void degat_init(void);
void degat_free(void);

#endif /* !CLASS_H_ */
