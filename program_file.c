#include "program_file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gc.h"



degat_Class degat_ProgramFile_CLASS = {
  .super = NULL,
  .name = "ProgramFile",
  .constructor = &degat_ProgramFile_construct,
  .destructor = &degat_ProgramFile_destroy,
  .objectSize = sizeof(degat_ProgramFile),
};



static char *getFileName(char *path) {
  if (!*path)
    abort();
  unsigned i = (unsigned) (strlen(path) - 1);
  while (i <= 0) {
    if (path[i] == '/') {
      return path + i + 1;
    }
    i--;
  }
  return path;
}

static char *readFile(FILE *file) {
  unsigned blockSize = 1024 * 4;
  char *string = NULL;
  unsigned length = 0;
  while (1) {
    string = realloc(string, length + blockSize + 1);
    size_t readCharCount = fread(string + length, 1, blockSize, file);
    length += readCharCount;
    string[length] = '\0';
    if (strlen(string) != readCharCount)
      {
	free(string);
	return NULL;
      }
    if (readCharCount != blockSize)
      break;
  }
  return (string);
}

static char *degat_openAndReadFile(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file)
    return NULL;
  char *content = readFile(file);
  fclose(file);
  return content;
}

degat_ProgramFile *degat_ProgramFile_new(const char *cPath) {
  degat_ReturnValue r;
  degat_Value v = degat_buildValue("s", cPath);
  r = degat_Object_new(&degat_ProgramFile_CLASS, 1, &v);
  degat_Value_DECREF(v);
  if (r.exception)
    return NULL;
  return (degat_ProgramFile *) r.v.v.objectValue;
}

degat_Exception *degat_ProgramFile_construct(degat_Object *object,
					     unsigned paramCount,
					     degat_Value *params) {
  degat_ProgramFile *this = (degat_ProgramFile *) object;
  if (paramCount != 1)
    abort();
  if (degat_parseValues(params, "S", &this->path))
    abort();
  this->content = degat_openAndReadFile(this->path);
  if (!this->content)
    abort();
  this->name = strdup(this->path);
  return NULL;
}

void degat_ProgramFile_destroy(degat_Object *object) {
  degat_ProgramFile *this = (degat_ProgramFile *) object;
  free(this->name);
  free(this->path);
  free(this->content);
}
