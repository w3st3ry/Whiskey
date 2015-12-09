#ifndef PROGRAM_FILE_H_
# define PROGRAM_FILE_H_

# include "object.h"

extern degat_Class degat_ProgramFile_CLASS;

void degat_ProgramFile_initClass(void);
void degat_ProgramFile_freeClass(void);



struct degat_ProgramFile_s;
typedef struct degat_ProgramFile_s degat_ProgramFile;

struct degat_ProgramFile_s {
  degat_OBJECT_HEAD

  char *name;
  char *path;
  char *content;
};



degat_ProgramFile *degat_ProgramFile_new(const char *path);

degat_Exception *degat_ProgramFile_construct(degat_Object *this,
					     unsigned paramCount,
					     degat_Value *params);
void degat_ProgramFile_destroy(degat_Object *this);



#endif /* !PROGRAM_FILE_H_ */
