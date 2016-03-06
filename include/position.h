#ifndef L_POSITION_H_
# define L_POSITION_H_

# include <stdio.h>
# include <stdbool.h>

struct wsky_ProgramFile_s;
#define wsky_ProgramFile struct wsky_ProgramFile_s

/**
 * Represents a position in a wsky_ProgramFile
 */
typedef struct wsky_Position_s {

  /** The file */
  wsky_ProgramFile *file;

  /** The 0-based character index */
  int index;

  /** The 1-based line number */
  int line;

  /** The 0-based column number */
  int column;
} wsky_Position;

extern const wsky_Position wsky_Position_UNKNOWN;

bool wsky_Position_isUnknown(const wsky_Position *position);

bool wsky_Position_equals(const wsky_Position *a, const wsky_Position *b);

void wsky_Position_print(const wsky_Position *self, FILE *file);

/**
 * Returns a new malloc'd string
 */
char *wsky_Position_toString(const wsky_Position *self);

#undef wsky_ProgramFile


#endif /* !L_POSITION_H_ */
