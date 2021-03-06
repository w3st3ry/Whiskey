#ifndef YOLO_H_
# define YOLO_H_

#include <stdbool.h>



typedef struct {
  int fail_count;
  int success_count;
} yolo_state_t;

extern yolo_state_t yolo_state;



# define YOLO__STR(s) #s
# define YOLO__XSTR(s) YOLO__STR(s)

# define YOLO__POSITION_STRING  (__FILE__ ", line " YOLO__XSTR(__LINE__))



# define yolo_fail() yolo_fail_impl(__func__, YOLO__POSITION_STRING)

# define yolo_assert(a) yolo_assert_impl((a), __func__, YOLO__POSITION_STRING)

# define yolo_assert_eq(type, expected, result)                         \
  yolo_assert_ ## type  ## _eq_impl(expected, result,                   \
                                    __func__, YOLO__POSITION_STRING)

# define yolo_assert_neq(type, expected, result)                        \
  yolo_assert_ ## type  ## _neq_impl(expected, result,                  \
                                     __func__, YOLO__POSITION_STRING)

# define yolo_assert_char_eq(expected, result)  \
  yolo_assert_eq(char, expected, result)
# define yolo_assert_int_eq(expected, result)   \
  yolo_assert_eq(int, expected, result)
# define yolo_assert_long_eq(expected, result)  \
  yolo_assert_eq(long, expected, result)

# define yolo_assert_uchar_eq(expected, result) \
  yolo_assert_eq(uchar, expected, result)
# define yolo_assert_uint_eq(expected, result)  \
  yolo_assert_eq(uint, expected, result)
# define yolo_assert_ulong_eq(expected, result) \
  yolo_assert_eq(ulong, expected, result)

# define yolo_assert_float_eq(expected, result) \
  yolo_assert_eq(float, expected, result)
# define yolo_assert_double_eq(expected, result)        \
  yolo_assert_eq(double, expected, result)

# define yolo_assert_ptr_eq(expected, result)   \
  yolo_assert_eq(ptr, expected, result)


# define yolo_assert_str_eq(expected, result)   \
  yolo_assert_eq(str, expected, result)



# define yolo_assert_char_neq(expected, result) \
  yolo_assert_neq(char, expected, result)
# define yolo_assert_int_neq(expected, result)  \
  yolo_assert_neq(int, expected, result)
# define yolo_assert_long_neq(expected, result) \
  yolo_assert_neq(long, expected, result)

# define yolo_assert_uchar_neq(expected, result)        \
  yolo_assert_neq(uchar, expected, result)
# define yolo_assert_uint_neq(expected, result) \
  yolo_assert_neq(uint, expected, result)
# define yolo_assert_ulong_neq(expected, result)        \
  yolo_assert_neq(ulong, expected, result)

# define yolo_assert_ptr_neq(expected, result)  \
  yolo_assert_neq(ptr, expected, result)

# define yolo_assert_null(a)                    \
  yolo_assert_ptr_eq(0, (a))
# define yolo_assert_not_null(a)                \
  yolo_assert_ptr_neq(0, (a))


# define yolo_assert_str_neq(expected, result)  \
  yolo_assert_neq(str, expected, result)



void yolo_fail_impl(const char *test_name, const char *position);
void yolo_assert_impl(bool a, const char *test_name, const char *position);


#define YOLO__ASSERT_EQ_DECL(T, type_name)                              \
  void yolo_assert_ ## type_name ## _eq_impl(T expected, T result,      \
                                             const char *test_name,     \
                                             const char *position);     \
  void yolo_assert_ ## type_name ## _neq_impl(T expected, T result,     \
                                              const char *test_name,    \
                                              const char *position);

YOLO__ASSERT_EQ_DECL(char, char)
YOLO__ASSERT_EQ_DECL(int, int)
YOLO__ASSERT_EQ_DECL(long, long)
YOLO__ASSERT_EQ_DECL(float, float)
YOLO__ASSERT_EQ_DECL(double, double)

YOLO__ASSERT_EQ_DECL(unsigned char, uchar)
YOLO__ASSERT_EQ_DECL(unsigned int, uint)
YOLO__ASSERT_EQ_DECL(unsigned long, ulong)

YOLO__ASSERT_EQ_DECL(void *, ptr)
YOLO__ASSERT_EQ_DECL(const char *, str)

void yolo_begin(void);
void yolo_end(void);

#endif /* !YOLO_H_ */
