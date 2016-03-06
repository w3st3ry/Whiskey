#ifndef PATH_H
# define PATH_H


/**
 * Gets an absolute path from a relative one.
 *
 * Returns a malloced string or NULL on error.
 */
char *wsky_path_getAbsolutePath(const char *relative);

/**
 * Returns the directory path.
 *
 * Returns a malloced string.
 */
char *wsky_path_getDirectoryPath(const char *path);

char *wsky_path_concat(const char *left, const char *right);

char *wsky_path_getCurrentDirectory(void);

const char *wsky_path_getFileName(const char *path);

char *wsky_path_getProgramDirectoryPath(void);
char *wsky_path_getProgramPath(void);

char *wsky_path_removeExtension(const char *fileName);

#endif /* PATH_H */
