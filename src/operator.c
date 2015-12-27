#include "operator.h"

const char *wsky_Operator_toString(wsky_Operator operator) {
# define CASE(name, op) case wsky_Operator_ ## name: return #op

  switch (operator) {
    CASE(PLUS, +); CASE(MINUS, -);
    CASE(STAR, *); CASE(SLASH, /);
    CASE(EQUALS, ==); CASE(NOT_EQUALS, !=);
    CASE(LT, <); CASE(LT_EQ, <=);
    CASE(GT, >); CASE(GT_EQ, >=);

    CASE(AND, and); CASE(OR, or); CASE(NOT, not);
  }

  return "op";
# undef CASE
}
