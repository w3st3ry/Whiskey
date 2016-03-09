#ifndef HEAP_H
#define HEAP_H


# include "objects/attribute_error.h"
# include "objects/class.h"
# include "objects/exception.h"
# include "objects/float.h"
# include "objects/function.h"
# include "objects/import_error.h"
# include "objects/instance_method.h"
# include "objects/method.h"
# include "objects/module.h"
# include "objects/name_error.h"
# include "objects/not_implemented_error.h"
# include "objects/exception.h"
# include "objects/object.h"
# include "objects/parameter_error.h"
# include "objects/str.h"
# include "objects/syntax_error_ex.h"
# include "objects/type_error.h"
# include "objects/value_error.h"

# define IMPORT(name) typedef wsky_##name name;

IMPORT(AttributeError)
IMPORT(Class)
IMPORT(Exception)
IMPORT(Function)
IMPORT(ImportError)
IMPORT(InstanceMethod)
IMPORT(Method)
IMPORT(Module)
IMPORT(NameError)
IMPORT(NotImplementedError)
IMPORT(Object)
IMPORT(ParameterError)
IMPORT(String)
IMPORT(SyntaxError)
IMPORT(TypeError)
IMPORT(ValueError)


bool wsky_heaps_contains(void *pointer);

Object *wsky_heaps_allocateObject(void);

void wsky_heaps_unmark(void);

void wsky_heaps_deleteUnmarkedObjects(void);

/**
 * Frees everything.
 */
void wsky_heaps_free(void);


#endif /* HEAP_H */
