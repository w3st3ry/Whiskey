/* Included in eval.c */

#define OP_TEMPLATE(op, opName)                                 \
  static ReturnValue int##opName(wsky_int left, Value right) {  \
    if (isInt(right)) {                                         \
      RETURN_INT(left op right.v.intValue);                     \
    }                                                           \
    if (isFloat(right)) {                                       \
      RETURN_FLOAT(left op right.v.floatValue);                 \
    }                                                           \
    RETURN_NOT_IMPL(#op);                                       \
  }

OP_TEMPLATE(+, Plus)
OP_TEMPLATE(-, Minus)
OP_TEMPLATE(*, Star)
OP_TEMPLATE(/, Slash)

#undef OP_TEMPLATE


#define OP_TEMPLATE(op, opName)                                 \
  static ReturnValue int##opName(wsky_int left, Value right) {  \
    if (isInt(right)) {                                         \
      RETURN_BOOL(left op right.v.intValue);                    \
    }                                                           \
    if (isFloat(right)) {                                       \
      RETURN_BOOL(left op right.v.floatValue);                  \
    }                                                           \
    RETURN_NOT_IMPL(#op);                                       \
  }

OP_TEMPLATE(<, LT)
OP_TEMPLATE(>, GT)

#undef OP_TEMPLATE

#define OP_TEMPLATE(op, opName)                                 \
  static ReturnValue int##opName(wsky_int left, Value right) {  \
    if (isInt(right)) {                                         \
      RETURN_BOOL(left op right.v.intValue);                    \
    }                                                           \
    RETURN_NOT_IMPL(#op);                                       \
  }

OP_TEMPLATE(<=, LTE)
OP_TEMPLATE(>=, GTE)

#undef OP_TEMPLATE


static ReturnValue evalBinOperatorInt(wsky_int left,
                                      wsky_Operator operator,
                                      Value right) {

  switch (operator) {
  case wsky_Operator_PLUS: return intPlus(left, right);
  case wsky_Operator_MINUS: return intMinus(left, right);
  case wsky_Operator_STAR: return intStar(left, right);
  case wsky_Operator_SLASH: return intSlash(left, right);

  case wsky_Operator_EQUALS:
    if (isInt(right)) {
      RETURN_BOOL(left == right.v.intValue);
    }
    RETURN_NOT_IMPL(wsky_Operator_toString(operator));

  case wsky_Operator_NOT_EQUALS:
    if (isInt(right)) {
      RETURN_BOOL(left != right.v.intValue);
    }
    RETURN_NOT_IMPL(wsky_Operator_toString(operator));

  case wsky_Operator_LT: return intLT(left, right);
  case wsky_Operator_LT_EQ: return intLTE(left, right);
  case wsky_Operator_GT: return intGT(left, right);
  case wsky_Operator_GT_EQ: return intGTE(left, right);

  default:
    break;
  }

  RETURN_NOT_IMPL(wsky_Operator_toString(operator));
}


static ReturnValue evalUnaryOperatorInt(wsky_Operator operator,
                                        wsky_int right) {
  switch (operator) {
  case wsky_Operator_PLUS: RETURN_INT(right);
  case wsky_Operator_MINUS: RETURN_INT(-right);

  default:
    break;
  }

  RETURN_NOT_IMPL(wsky_Operator_toString(operator));
}
