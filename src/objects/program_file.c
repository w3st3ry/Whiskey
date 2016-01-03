#include "objects/program_file.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "objects/exception.h"
#include "objects/parameter_error.h"
#include "gc.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_Exception Exception;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_ProgramFile ProgramFile;


static wsky_ReturnValue construct(Object *object,
                                  unsigned paramCount,
                                  Value *params);
static wsky_ReturnValue destroy(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_ProgramFile_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "ProgramFile",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(wsky_ProgramFile),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};



static char *getFileName(char *path) {
  assert(*path);
  long i = (long)strlen(path) - 1;
  while (i >= 0) {
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
    string = wsky_REALLOC(string, length + blockSize + 1);
    size_t readCharCount = fread(string + length, 1, blockSize, file);
    length += readCharCount;
    string[length] = '\0';
    if (strlen(string) != length) {
      wsky_FREE(string);
      return NULL;
    }
    if (readCharCount != blockSize)
      break;
  }
  return string;
}

static char *wsky_openAndReadFile(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file)
    return NULL;
  char *content = readFile(file);
  fclose(file);
  return content;
}

ProgramFile *wsky_ProgramFile_new(const char *cPath) {
  Value v = wsky_buildValue("s", cPath);
  ReturnValue r = wsky_Object_new(wsky_ProgramFile_CLASS, 1, &v);
  if (r.exception)
    return NULL;
  return (ProgramFile *) r.v.v.objectValue;
}

static wsky_ReturnValue construct(Object *object,
                            unsigned paramCount,
                            Value *params) {
  if (paramCount != 1)
    wsky_RETURN_NEW_PARAMETER_ERROR("Parameter error");

  ProgramFile *self = (ProgramFile *) object;
  if (wsky_parseValues(params, "S", &self->path))
    wsky_RETURN_NEW_PARAMETER_ERROR("Parameter error");
  self->content = wsky_openAndReadFile(self->path);
  if (!self->content)
    wsky_RETURN_NEW_EXCEPTION("IO error");
  self->name = wsky_STRDUP(getFileName(self->path));
  wsky_RETURN_NULL;
}

static wsky_ReturnValue destroy(Object *object) {
  ProgramFile *self = (ProgramFile *) object;
  wsky_FREE(self->name);
  wsky_FREE(self->path);
  wsky_FREE(self->content);
  wsky_RETURN_NULL;
}
