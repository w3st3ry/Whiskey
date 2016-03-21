#include "whiskey_private.h"

const char *wsky_Operator_toString(Operator operator) {
# define C(name, op) case wsky_Operator_ ## name: return #op

  switch (operator) {

  case wsky_Operator_LEFT_PAREN: return "(";
  case wsky_Operator_RIGHT_PAREN: return ")";
  case wsky_Operator_COMMA: return ",";

    C(COLON, :); C(SEMICOLON, ;);
    C(DOT, .);
    C(LEFT_BRACKET, [); C(RIGHT_BRACKET, ]);
    C(LEFT_BRACE, {); C(RIGHT_BRACE, });

    C(PLUS, +); C(MINUS, -);
    C(STAR, *); C(SLASH, /);

    C(PLUS_EQ, +); C(MINUS_EQ, -);
    C(STAR_EQ, *); C(SLASH_EQ, /);

    C(EQUALS, ==); C(NOT_EQUALS, !=);
    C(LT, <); C(LT_EQ, <=);
    C(GT, >); C(GT_EQ, >=);

    C(AND, and); C(OR, or); C(NOT, not);

    C(ASSIGN, =);

    C(AT, @);
  }

  abort();
# undef C
}
