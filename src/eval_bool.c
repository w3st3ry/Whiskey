/* Included in eval.c */


static ReturnValue boolAnd(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left && right.v.intValue);
  }
  return wsky_ReturnValue_FALSE;
}

static ReturnValue boolOr(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left || right.v.boolValue);
  }
  return wsky_ReturnValue_TRUE;
}


static ReturnValue boolEquals(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left == right.v.intValue);
  }
  return wsky_ReturnValue_FALSE;
}

static ReturnValue boolNotEquals(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left != right.v.boolValue);
  }
  return wsky_ReturnValue_TRUE;
}


static ReturnValue evalBinOperatorBool(bool left,
                                       wsky_Operator operator,
                                       Value right) {

  switch (operator) {
  case wsky_Operator_EQUALS: return boolEquals(left, right);
  case wsky_Operator_NOT_EQUALS: return boolNotEquals(left, right);

  case wsky_Operator_AND: return boolAnd(left, right);
  case wsky_Operator_OR: return boolOr(left, right);

  default:
    break;
  }

  RETURN_NOT_IMPL(wsky_Operator_toString(operator));
}

static ReturnValue evalUnaryOperatorBool(wsky_Operator operator,
                                         bool right) {
  switch (operator) {
  case wsky_Operator_NOT: RETURN_BOOL(!right);

  default:
    break;
  }

  RETURN_NOT_IMPL(wsky_Operator_toString(operator));
}
