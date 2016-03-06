#include "objects/program_file.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "objects/exception.h"
#include "objects/parameter_error.h"
#include "gc.h"
#include "path.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_Exception Exception;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_ProgramFile ProgramFile;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_ProgramFile_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "ProgramFile",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(wsky_ProgramFile),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_ProgramFile_CLASS;



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
  unsigned blockSize = 1024 * 1024;
  char *string = NULL;
  unsigned length = 0;
  while (1) {
    string = wsky_realloc(string, length + blockSize + 1);
    if (!string)
      return NULL;
    size_t readCharCount = fread(string + length, 1, blockSize, file);
    length += readCharCount;
    string[length] = '\0';
    if (strlen(string) != length) {
      wsky_free(string);
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

ReturnValue wsky_ProgramFile_new(const char *cPath) {
  Value v = wsky_buildValue("s", cPath);
  ReturnValue rv = wsky_Object_new(wsky_ProgramFile_CLASS, 1, &v);
  return rv;
}

ProgramFile *wsky_ProgramFile_getUnknown(void) {
  ReturnValue rv = wsky_Object_new(wsky_ProgramFile_CLASS, 0, NULL);
  assert(!rv.exception);
  return (ProgramFile *)rv.v.v.objectValue;
}

static void initUnknownFile(ProgramFile *self) {
  self->name = wsky_strdup("<unknown file>");
  self->absolutePath = NULL;
  self->directoryPath = wsky_path_getCurrentDirectory();
  self->content = NULL;
}

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  if (paramCount > 1)
    wsky_RETURN_NEW_PARAMETER_ERROR("Parameter error");

  ProgramFile *self = (ProgramFile *) object;

  if (paramCount == 0) {
    initUnknownFile(self);
    wsky_RETURN_NULL;
  }

  char *path;
  if (wsky_parseValues(params, "S", &path))
    wsky_RETURN_NEW_PARAMETER_ERROR("Parameter error");
  self->absolutePath = wsky_path_getAbsolutePath(path);
  wsky_free(path);
  if (!self->absolutePath)
    wsky_RETURN_NEW_EXCEPTION("Invalid path");

  self->content = wsky_openAndReadFile(self->absolutePath);
  if (!self->content) {
    wsky_free(self->absolutePath);
    wsky_RETURN_NEW_EXCEPTION("IO error");
  }

  char *dirAbsPath = wsky_path_getDirectoryPath(self->absolutePath);
  self->directoryPath = (dirAbsPath ?
                         dirAbsPath : wsky_path_getCurrentDirectory());

  self->name = wsky_strdup(getFileName(self->absolutePath));
  wsky_RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  ProgramFile *self = (ProgramFile *) object;
  wsky_free(self->name);
  wsky_free(self->absolutePath);
  wsky_free(self->directoryPath);
  wsky_free(self->content);
  wsky_RETURN_NULL;
}
