#include "minunit.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../program_file.h"



void testSetup() {
  degat_init();
}

void testTeardown() {
  degat_free();
}



MU_TEST(base) {
  degat_ProgramFile *pf = degat_ProgramFile_new("str.c");
  mu_assert(pf != NULL, "");
  if (!pf)
    return;
  degat_Object_DECREF(pf);
}



int main() {
  MU_SUITE_CONFIGURE(&testSetup, &testTeardown);
  MU_RUN_TEST(base);
}
