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
 * @param file A file or NULL if this is a builtin module
 */
wsky_Module *wsky_Module_new(const char *name, bool builtin,
                             wsky_ProgramFile *file);

/**
 * Creates a "main" module.
 */
wsky_Module *wsky_Module_newMain(void);

void wsky_Module_addValue(wsky_Module *module,
                          const char *name,
                          wsky_Value value);

/** A shortcut for wsky_Module_addValue() */
void wsky_Module_addObject(wsky_Module *module,
                           const char *name,
                           wsky_Object *object);

/** Another shortcut for wsky_Module_addValue() */
void wsky_Module_addFunction(wsky_Module *module,
                             const char *name,
                             int parameterCount,
                             wsky_Method0 function);


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
