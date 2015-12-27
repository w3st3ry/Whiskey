#ifndef EVAL_H_
# define EVAL_H_

#include "ast.h"
#include "objects/scope.h"

wsky_ReturnValue wsky_evalNode(const wsky_ASTNode *node, wsky_Scope *scope);
wsky_ReturnValue wsky_evalString(const char *source);

#endif /* !EVAL_H_ */
