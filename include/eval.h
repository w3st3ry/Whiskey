#ifndef EVAL_H_
# define EVAL_H_

#include "ast.h"
#include "objects/scope.h"

wsky_Result wsky_doBinaryOperation(wsky_Value left,
                                   wsky_Operator operator,
                                   wsky_Value right);

wsky_Result wsky_doUnaryOperation(wsky_Operator operator,
                                  wsky_Value right);

wsky_Result wsky_evalNode(const wsky_ASTNode *node, wsky_Scope *scope);

/**
 * @param scope The root scope or NULL. It is pushed and poped.
 */
wsky_Result wsky_evalString(const char *source, wsky_Scope *scope);

/**
 * @param scope The root scope or NULL. It is pushed and poped.
 */
wsky_Result wsky_evalFile(const char *filePath, wsky_Scope *scope);

wsky_Result wsky_evalModuleFile(const char *filePath);

/**
 * For the garbage collector.
 */
void wsky_eval_visitScopeStack(void);

void wsky_eval_pushScope(wsky_Scope *scope);
wsky_Scope *wsky_eval_popScope(void);

/**
 * Evaluates a sequence with the given inner scope.
 */
wsky_Result wsky_evalSequence(const wsky_SequenceNode *node,
                              wsky_Scope *innerScope);

#endif /* !EVAL_H_ */
