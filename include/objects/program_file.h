#ifndef PROGRAM_FILE_H_
# define PROGRAM_FILE_H_

# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup ProgramFile ProgramFile
 * @{
 */

extern const wsky_ClassDef wsky_ProgramFile_CLASS_DEF;

extern wsky_Class *wsky_ProgramFile_CLASS;


/**
 * A ProgramFile.
 *
 * Represents a Whiskey script or a Whiskey template.
 */
typedef struct wsky_ProgramFile_s {
  wsky_OBJECT_HEAD

  /** The name of the file */
  char *name;

  /** The full path of the file (can be NULL) */
  char *path;

  /** The content of the file (can be NULL) */
  char *content;
} wsky_ProgramFile;


wsky_ReturnValue wsky_ProgramFile_new(const char *path);

/** Returns an unknown file */
wsky_ProgramFile *wsky_ProgramFile_getUnknown(void);


/**
 * @}
 * @}
 */

#endif /* !PROGRAM_FILE_H_ */
