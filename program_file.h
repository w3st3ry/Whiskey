#ifndef FILE_H_
# define FILE_H_

# include "degat.h"

typedef struct {
  degat_OBJECT_HEAD

  char *name;
  char *path;
  char *content;
} degat_ProgramFile;

extern const degat_Class degat_ProgramFile_CLASS;

int degat_ProgramFile_read(const char *path, degat_ProgramFile **file);

#endif /* !FILE_H_ */
