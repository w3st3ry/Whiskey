/* Included in eval.c */


static Result boolAnd(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left && right.v.intValue);
  }
  RETURN_NOT_IMPL("and");
}

static Result boolOr(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left || right.v.boolValue);
  }
  RETURN_NOT_IMPL("or");
}


static Result boolEquals(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left == right.v.intValue);
  }
  RETURN_NOT_IMPL("==");
}

static Result boolNotEquals(bool left, Value right) {
  if (isBool(right)) {
    RETURN_BOOL(left != right.v.boolValue);
  }
  RETURN_NOT_IMPL("!=");
}


static Result evalBinOperatorBool(bool left,
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

static Result evalUnaryOperatorBool(wsky_Operator operator,
                                         bool right) {
  switch (operator) {
  case wsky_Operator_NOT: RETURN_BOOL(!right);

  default:
    break;
  }

  RETURN_NOT_IMPL(wsky_Operator_toString(operator));
}
