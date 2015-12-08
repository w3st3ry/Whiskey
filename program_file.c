#include "program_file.h"
#include "return_value.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const degat_Class degat_ProgramFile_CLASS = {
  .super = NULL,
  .name = "ProgramFile",
  .objectSize = sizeof(degat_ProgramFile),
};

degat_ReturnValue degat_ProgramFile_new(const char *path) {
  degat_ReturnValue r = degat_Object_new(&degat_ProgramFile_CLASS,
					 0,
					 NULL);
  degat_RETURN_INT(0);
}

degat_Exception *degat_ProgramFile_construct(degat_Object *object,
					     unsigned paramCount,
					     degat_Value *params) {
  degat_ProgramFile *this = (degat_ProgramFile *) object;
  this->name = strdup("yolo");
  return (NULL);
}

void degat_ProgramFile_destroy(degat_Object *object) {
  degat_ProgramFile *this = (degat_ProgramFile *) object;
  free(this->name);
}

/*
int degat_ProgramFile_read(const char *path,
			   degat_ProgramFile **file_pointer) {

  *file_pointer = malloc(sizeof(degat_ProgramFile));

  (*file_pointer)->path = strdup(path);
  if (!*file_pointer)
    return 1;
  FILE *file = fopen(path, "r");
  fclose(file);
  return 0;
}
*/
