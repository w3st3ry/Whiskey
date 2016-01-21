#include "yolo.h"

#include <stdio.h>
#include <string.h>

yolo_state_t yolo_state = {
  .fail_count = 0,
  .success_count = 0,
};

static void print_csi_sgr(int n) {
  printf("\x1b[%dm", n);
}

static void reset_attributes() {
  print_csi_sgr(0);
}

static void print_fail(const char *test_name, const char *position) {
  printf("\n");

  print_csi_sgr(95);
  printf("%s\n", position);
  reset_attributes();

  print_csi_sgr(31);
  print_csi_sgr(1);
  printf("%s failed", test_name);
  printf(": ");
  reset_attributes();
}

void yolo_fail_impl(const char *test_name, const char *position) {
  print_fail(test_name, position);
  printf("assertion error\n");
  yolo_state.fail_count++;
}

void yolo_assert_impl(bool a, const char *test_name, const char *position) {
  if (a) {
    putchar('.');
    yolo_state.success_count++;
    return;
  }
  yolo_fail_impl(test_name, position);
}

#define ASSERT_EQ_IMPL_TEMPLATE(T, type_name, format)                   \
  void yolo_assert_ ## type_name ## _eq_impl(T expected, T result,      \
                                             const char *test_name,     \
                                             const char *position) {    \
    if (expected == result)                                             \
      yolo_assert_impl(true, test_name, position);                      \
    else {                                                              \
      print_fail(test_name, position);                                  \
      printf("%" format " expected, got %" format " instead\n",         \
             expected, result);                                         \
      yolo_state.fail_count++;                                          \
    }                                                                   \
  }                                                                     \
                                                                        \
  void yolo_assert_ ## type_name ## _neq_impl(T expected, T result,     \
                                              const char *test_name,    \
                                              const char *position) {   \
    if (expected != result)                                             \
      yolo_assert_impl(true, test_name, position);                      \
    else {                                                              \
      print_fail(test_name, position);                                  \
      printf("got %" format "\n", result);                              \
      yolo_state.fail_count++;                                          \
    }                                                                   \
  }

void yolo_assert_str_eq_impl(const char *expected, const char *result,
                             const char *test_name,
                             const char *position) {
  if (strcmp(expected, result) == 0)
    yolo_assert_impl(true, test_name, position);
  else {
    print_fail(test_name, position);

    printf("'");

    print_csi_sgr(96);
    printf("%s", expected);
    reset_attributes();

    printf("' expected, got '");

    print_csi_sgr(91);
    printf("%s", result);
    reset_attributes();

    printf("' instead\n");
    yolo_state.fail_count++;
  }
}



ASSERT_EQ_IMPL_TEMPLATE(char, char, "c")
ASSERT_EQ_IMPL_TEMPLATE(int, int, "d")
ASSERT_EQ_IMPL_TEMPLATE(long, long, "ld")
ASSERT_EQ_IMPL_TEMPLATE(unsigned int, uint, "u")
ASSERT_EQ_IMPL_TEMPLATE(unsigned long, ulong, "lu")

ASSERT_EQ_IMPL_TEMPLATE(void *, ptr, "p")


void yolo_begin(void) {
}

void yolo_end(void) {
  int total = yolo_state.fail_count + yolo_state.success_count;
  float percent = ((float) yolo_state.success_count / total) * 100;

  printf("\n");

  print_csi_sgr(1);
  printf("%d", total);
  reset_attributes();

  printf(" tests, ");

  print_csi_sgr(1);
  printf("%d", yolo_state.fail_count);
  reset_attributes();

  printf(" failure(s) (");

  print_csi_sgr(1);
  printf("%.2f%%", percent);
  reset_attributes();

  printf(")\n");
}
