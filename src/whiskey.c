#include "whiskey.h"
#include "class_def.h"
#include "gc.h"
#include "modules/math.h"


void wsky_start(void) {
  wsky_initBuiltinClasses();
  wsky_math_init();
}


void wsky_stop(void) {
  wsky_GC_deleteAll();

  wsky_freeBuiltinClasses();
  wsky_Module_deleteModules();
}
