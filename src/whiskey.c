#include "whiskey.h"
#include "class_def.h"
#include "gc.h"
#include "modules/math.h"


static bool started = false;

bool wsky_isStarted(void) {
  return started;
}

void wsky_start(void) {
  wsky_GC_init();
  wsky_initBuiltinClasses();
  wsky_math_init();
  started = true;
}


void wsky_stop(void) {
  started = false;
  wsky_GC_deleteAll();

  wsky_freeBuiltinClasses();
  wsky_Module_deleteModules();
}
