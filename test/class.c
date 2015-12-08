#include "minunit.h"

#include "../class.h"

MU_TEST(yolo) {
	mu_check(7 == 7);
}

int main() {
  MU_RUN_TEST(yolo);
}
