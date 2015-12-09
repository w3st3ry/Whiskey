#include "yolo.h"

#include <stdio.h>
#include <stdbool.h>

yolo_state_t yolo_state = {
  .fail_count = 0,
  .success_count = 0,
};

static void print_csi_sgr(int n) {
  printf("\x1b[%dm", n);
}

static void print_fail(const char *test_name, const char *position) {
  printf("\n");
  printf("%s\n", position);
  print_csi_sgr(31);
  print_csi_sgr(1);
  printf("%s failed", test_name);
  printf(": ");
  print_csi_sgr(0);
}

void yolo_fail_impl(const char *test_name, const char *position) {
  print_fail(test_name, position);
  printf("assertion error\n");
  yolo_state.fail_count++;
}

void yolo_assert_impl(bool a, const char *test_name, const char *position) {
  if (a)
    {
      putchar('.');
      yolo_state.success_count++;
      return;
    }
  yolo_fail_impl(test_name, position);
}

void yolo_assert_char_eq_impl(char expected, char result,
			      const char *test_name, const char *position) {
  if (expected == result)
    yolo_assert_impl(true, test_name, position);
  else
    {
      print_fail(test_name, position);
      printf("%c expected, got %c instead\n", expected, result);
      yolo_state.fail_count++;
    }
}

void yolo_assert_int_eq_impl(int expected, int result,
			     const char *test_name, const char *position) {
  if (expected == result)
    yolo_assert_impl(true, test_name, position);
  else
    {
      print_fail(test_name, position);
      printf("%d expected, got %d instead\n", expected, result);
      yolo_state.fail_count++;
    }
}

void yolo_begin(void) {
}

void yolo_end(void) {
  int total = yolo_state.fail_count + yolo_state.success_count;
  float percent = ((float) yolo_state.success_count / total) * 100;
  printf("\n%d tests, %d failures (%.1f%%)\n",
	 total,
	 yolo_state.fail_count,
	 percent);
}
