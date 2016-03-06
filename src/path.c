#include "path.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include "gc.h"


char *wsky_path_getAbsolutePath(const char *relative) {
  // TODO: Use an alternative for non-POSIX systems.
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
  for (int i = strlen(path) - 1; i > 0 && path[i] == '/'; i--) {
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
