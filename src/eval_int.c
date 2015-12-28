/* Included in eval.c */

#define RETURN_UNSUPPORTED(operator, right)                             \
  return createUnsupportedBinOpError("Integer", #operator, right)


#define OP_TEMPLATE(op, opName)                                 \
  static ReturnValue int##opName(int64_t left, Value right) {   \
    if (IS_INT(right)) {                                        \
      wsky_RETURN_INT(left op right.v.intValue);                \
    }                                                           \
    if (IS_FLOAT(right)) {                                      \
      wsky_RETURN_FLOAT(left op right.v.floatValue);            \
    }                                                           \
    RETURN_UNSUPPORTED(op, right);                              \
  }

OP_TEMPLATE(+, Plus)
OP_TEMPLATE(-, Minus)
OP_TEMPLATE(*, Star)
OP_TEMPLATE(/, Slash)

#undef OP_TEMPLATE


#define OP_TEMPLATE(op, opName)                                 \
  static ReturnValue int##opName(int64_t left, Value right) {   \
    if (IS_INT(right)) {                                        \
      wsky_RETURN_BOOL(left op right.v.intValue);               \
    }                                                           \
    if (IS_FLOAT(right)) {                                      \
      wsky_RETURN_BOOL(left op right.v.floatValue);             \
    }                                                           \
    RETURN_UNSUPPORTED(op, right);                              \
  }

OP_TEMPLATE(<, LT)
OP_TEMPLATE(>, GT)
OP_TEMPLATE(<=, LTE)
OP_TEMPLATE(>=, GTE)

#undef OP_TEMPLATE


static ReturnValue intEquals(int64_t left, Value right) {
  if (IS_INT(right)) {
    wsky_RETURN_BOOL(left == right.v.intValue);
  }
  return wsky_ReturnValue_FALSE;
}

static ReturnValue intNotEquals(int64_t left, Value right) {
  if (IS_INT(right)) {
    wsky_RETURN_BOOL(left != right.v.intValue);
  }
  return wsky_ReturnValue_TRUE;
}


static ReturnValue evalBinOperatorInt(int64_t left,
                                      wsky_Operator operator,
                                      Value right) {

  switch (operator) {
  case wsky_Operator_PLUS: return intPlus(left, right);
  case wsky_Operator_MINUS: return intMinus(left, right);
  case wsky_Operator_STAR: return intStar(left, right);
  case wsky_Operator_SLASH: return intSlash(left, right);

  case wsky_Operator_EQUALS: return intEquals(left, right);
  case wsky_Operator_NOT_EQUALS: return intNotEquals(left, right);

  case wsky_Operator_LT: return intLT(left, right);
  case wsky_Operator_LT_EQ: return intLTE(left, right);
  case wsky_Operator_GT: return intGT(left, right);
  case wsky_Operator_GT_EQ: return intGTE(left, right);

  default:
    break;
  }

  return createUnsupportedBinOpError("Integer",
                                     wsky_Operator_toString(operator),
                                     right);
}

#undef RETURN_UNSUPPORTED
