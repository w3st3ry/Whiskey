#include "path.h"

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include "gc.h"
#include "string_utils.h"

// TODO: Add support for non-Linux systems
// TODO: Write tests

char *wsky_path_getAbsolutePath(const char *relative) {
  char *abs = realpath(relative, NULL);
  return abs;
}


char *wsky_path_getDirectoryPath(const char *path) {
  char *path2 = wsky_strdup(path);
  char *dir = wsky_strdup(dirname(path2));
  wsky_free(path2);
  return dir;
}

char *wsky_path_removeTrailingSeparators(const char *path) {
  char *newPath = wsky_strdup(path);
  ssize_t i;
  for (i = strlen(path) - 1; i > 0 && path[i] == '/'; i--) {
    newPath[i] = '\0';
  }
  return newPath;
}

char *wsky_path_concat(const char *left, const char *right) {
  char *left2 = wsky_path_removeTrailingSeparators(left);
  char *path = wsky_safeMalloc(strlen(left2) + strlen(right) + 2);
  strcpy(path, left2);
  strcat(path, "/");
  strcat(path, right);
  wsky_free(left2);
  return path;
}

char *wsky_path_getCurrentDirectory(void) {
  return getcwd(NULL, 0);
}

static ssize_t getLastIndexOf(const char *string, char c) {
  ssize_t i = (ssize_t)strlen(string) - 1;
  while (i >= 0) {
    if (string[i] == c)
      return i;
    i--;
  }
  return -1;
}

const char *wsky_path_getFileName(const char *path) {
  ssize_t i = getLastIndexOf(path, '/');
  if (i == -1)
    return NULL;
  return path + i + 1;
}

char *wsky_path_getProgramPath(void) {
  char linkPath[100];
  char buffer[1024] = {0};

  snprintf(linkPath, sizeof(linkPath) - 1, "/proc/%d/exe", getpid());
  if (readlink(linkPath, buffer, sizeof(buffer) - 1) == 1)
    abort();
  return wsky_strdup(buffer);
}

char *wsky_path_getProgramDirectoryPath(void) {
  char *program = wsky_path_getProgramPath();
  if (!program)
    return NULL;
  char *dir = wsky_path_getDirectoryPath(program);
  wsky_free(program);
  return dir;
}

char *wsky_path_removeExtension(const char *fileName) {
  ssize_t i = getLastIndexOf(fileName, '.');
  if (i == -1)
    return wsky_strdup(fileName);
  return wsky_strndup(fileName, i);
}
