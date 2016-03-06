/* Included in eval.c */


#define OP_TEMPLATE(op, opName)                                         \
  static ReturnValue float##opName(wsky_float left, Value right) {      \
    if (isInt(right)) {                                                 \
      RETURN_FLOAT(left op right.v.intValue);                      \
    }                                                                   \
    if (isFloat(right)) {                                               \
      RETURN_FLOAT(left op right.v.floatValue);                    \
    }                                                                   \
    RETURN_NOT_IMPL(#op);                                               \
  }

OP_TEMPLATE(+, Plus)
OP_TEMPLATE(-, Minus)
OP_TEMPLATE(*, Star)
OP_TEMPLATE(/, Slash)

#undef OP_TEMPLATE


#define OP_TEMPLATE(op, opName)                                         \
  static ReturnValue float##opName(wsky_float left, Value right) {      \
    if (isInt(right)) {                                                 \
      RETURN_BOOL(left op right.v.intValue);                       \
    }                                                                   \
    if (isFloat(right)) {                                               \
      RETURN_BOOL(left op right.v.floatValue);                     \
    }                                                                   \
    RETURN_NOT_IMPL(#op);                                               \
  }

OP_TEMPLATE(<, LT)
OP_TEMPLATE(>, GT)
OP_TEMPLATE(<=, LTE)
OP_TEMPLATE(>=, GTE)

#undef OP_TEMPLATE


static ReturnValue evalBinOperatorFloat(wsky_float left,
                                        wsky_Operator operator,
                                        Value right) {

  switch (operator) {
  case wsky_Operator_PLUS: return floatPlus(left, right);
  case wsky_Operator_MINUS: return floatMinus(left, right);
  case wsky_Operator_STAR: return floatStar(left, right);
  case wsky_Operator_SLASH: return floatSlash(left, right);

  case wsky_Operator_LT: return floatLT(left, right);
  case wsky_Operator_LT_EQ: return floatLTE(left, right);
  case wsky_Operator_GT: return floatGT(left, right);
  case wsky_Operator_GT_EQ: return floatGTE(left, right);

  default:
    break;
  }

  return createUnsupportedBinOpError("Float",
                                     wsky_Operator_toString(operator),
                                     right);
}

#undef RETURN_UNSUPPORTED



static ReturnValue evalUnaryOperatorFloat(wsky_Operator operator,
                                          wsky_float right) {
  switch (operator) {
  case wsky_Operator_PLUS: RETURN_FLOAT(right);
  case wsky_Operator_MINUS: RETURN_FLOAT(-right);

  default:
    break;
  }

  return createUnsupportedUnaryOpError(wsky_Operator_toString(operator),
                                       "Float");
}
