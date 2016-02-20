#ifndef EVAL_H_
# define EVAL_H_

#include "ast.h"
#include "objects/scope.h"

wsky_ReturnValue wsky_doBinaryOperation(wsky_Value left,
                                        wsky_Operator operator,
                                        wsky_Value right);

wsky_ReturnValue wsky_doUnaryOperation(wsky_Operator operator,
                                       wsky_Value right);

wsky_ReturnValue wsky_evalNode(const wsky_ASTNode *node, wsky_Scope *scope);
wsky_ReturnValue wsky_evalString(const char *source);
wsky_ReturnValue wsky_evalFile(const char *filePath);

#endif /* !EVAL_H_ */
