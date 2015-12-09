#include "minunit.h"

#include "../position.h"

void testSetup() {
  degat_init();
}

void testTeardown() {
  degat_free();
}

int main() {
  MU_SUITE_CONFIGURE(&testSetup, &testTeardown);
}
