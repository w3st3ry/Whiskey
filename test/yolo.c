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

#define ASSERT_EQ_IMPL_TEMPLATE(T, type_name, format)			\
  void yolo_assert_ ## type_name ## _eq_impl(T expected, T result,	\
					     const char *test_name,	\
					     const char *position) {	\
    if (expected == result)						\
      yolo_assert_impl(true, test_name, position);			\
    else								\
      {									\
	print_fail(test_name, position);				\
	printf("%" format " expected, got %" format " instead\n",	\
	       expected, result);					\
	yolo_state.fail_count++;					\
      }									\
  }									\
									\
  void yolo_assert_ ## type_name ## _neq_impl(T expected, T result,	\
					      const char *test_name,	\
					      const char *position) {	\
    if (expected != result)						\
      yolo_assert_impl(true, test_name, position);			\
    else								\
      {									\
	print_fail(test_name, position);				\
	printf("got %" format "\n", result);				\
	yolo_state.fail_count++;					\
      }									\
  }

void yolo_assert_str_eq_impl(const char *expected, const char *result,
			     const char *test_name,
			     const char *position) {
  if (strcmp(expected, result) == 0)
    yolo_assert_impl(true, test_name, position);
  else
    {
      print_fail(test_name, position);
      printf("'%s' expected, got '%s' instead\n",
	     expected, result);
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
  printf("\n%d tests, %d failure(s) (%.1f%%)\n",
	 total,
	 yolo_state.fail_count,
	 percent);
}
