#ifndef MODULE_H
# define MODULE_H

# include "class_def.h"
# include "program_file.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Module Module
 * @{
 */


extern const wsky_ClassDef wsky_Module_CLASS_DEF;

extern wsky_Class *wsky_Module_CLASS;


/** A Whiskey Module */
typedef struct wsky_Module_s {
  wsky_OBJECT_HEAD

  /** Read-only */
  char *name;

  /** Read-only */
  bool builtin;

  wsky_Dict members;

  /** The file of the module */
  wsky_ProgramFile *file;
} wsky_Module;


/**
 * Creates a new Module.
 * @param file A file or NULL
 */
wsky_Module *wsky_Module_new(const char *name, wsky_Dict *members,
                             bool builtin,
                             wsky_ProgramFile *file);


typedef struct wsky_ModuleList_s {
  wsky_Module *module;
  struct wsky_ModuleList_s *next;
} wsky_ModuleList;

/** Returns the list of the loaded modules */
wsky_ModuleList *wsky_Module_getModules(void);

/** Deletes the internal module list. Used by wsky_stop(). */
void wsky_Module_deleteModules(void);

/**
 * @}
 * @}
 */

#endif /* MODULE_H */
