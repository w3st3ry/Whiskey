#include "objects/program_file.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "objects/exception.h"
#include "gc.h"



static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params);
static void destroy(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0},
};

wsky_Class wsky_ProgramFile_CLASS = {
  .super = NULL,
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

wsky_ProgramFile *wsky_ProgramFile_new(const char *cPath) {
  wsky_ReturnValue r;
  wsky_Value v = wsky_buildValue("s", cPath);
  r = wsky_Object_new(&wsky_ProgramFile_CLASS, 1, &v);
  if (r.exception)
    return NULL;
  return (wsky_ProgramFile *) r.v.v.objectValue;
}

static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params) {
  if (paramCount != 1)
    return wsky_Exception_new("Parameter error", NULL);
  wsky_ProgramFile *this = (wsky_ProgramFile *) object;
  if (wsky_parseValues(params, "S", &this->path))
    return wsky_Exception_new("Parameter error", NULL);
  this->content = wsky_openAndReadFile(this->path);
  if (!this->content)
    return wsky_Exception_new("IO error", NULL);
  this->name = wsky_STRDUP(getFileName(this->path));
  return NULL;
}

static void destroy(wsky_Object *object) {
  wsky_ProgramFile *this = (wsky_ProgramFile *) object;
  wsky_FREE(this->name);
  wsky_FREE(this->path);
  wsky_FREE(this->content);
}
