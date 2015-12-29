#ifndef PROGRAM_FILE_H_
# define PROGRAM_FILE_H_

# include "object.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup ProgramFile ProgramFile
 * @{
 */

extern wsky_Class wsky_ProgramFile_CLASS;


/**
 * A ProgramFile.
 *
 * Represents a Whiskey script or a Whiskey template.
 */
typedef struct wsky_ProgramFile_s wsky_ProgramFile;
struct wsky_ProgramFile_s {
  wsky_OBJECT_HEAD

  /** The name of the file */
  char *name;

  /** The full path of the file */
  char *path;

  /** The content of the file */
  char *content;
};


wsky_ProgramFile *wsky_ProgramFile_new(const char *path);

/**
 * @}
 * @}
 */

#endif /* !PROGRAM_FILE_H_ */
