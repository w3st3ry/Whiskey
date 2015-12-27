#ifndef PROGRAM_FILE_H_
# define PROGRAM_FILE_H_

# include "object.h"

extern wsky_Class wsky_ProgramFile_CLASS;

void wsky_ProgramFile_initClass(void);
void wsky_ProgramFile_freeClass(void);



struct wsky_ProgramFile_s;
typedef struct wsky_ProgramFile_s wsky_ProgramFile;

struct wsky_ProgramFile_s {
  wsky_OBJECT_HEAD

  char *name;
  char *path;
  char *content;
};


wsky_ProgramFile *wsky_ProgramFile_new(const char *path);


#endif /* !PROGRAM_FILE_H_ */
