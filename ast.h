#ifndef AST_H_
# define AST_H_

/* Abstract Syntax Tree */

#include "token.h"

typedef enum {

  /* Literals */
  wsky_ASTNode_INTEGER,
  wsky_ASTNode_FLOAT,
  wsky_ASTNode_STRING,
  wsky_ASTNode_ARRAY,

  wsky_ASTNode_IDENTIFIER,

  wsky_ASTNode_PARENS,

  wsky_ASTNode_FUNCTION,

  wsky_ASTNode_ASSIGN,

  wsky_ASTNode_UNARY_OPERATION,
  wsky_ASTNode_BINARY_OPERATION,

} wsky_ASTNodeType;



#define wsky_ASTNode_HEAD \
  wsky_ASTNodeType type;  \
  wsky_Token token;


typedef struct {
  wsky_ASTNode_HEAD
} wsky_ASTNode;


char *wsky_ASTNode_toString(const wsky_ASTNode *node);
void wsky_ASTNode_print(const wsky_ASTNode *node, FILE *output);
void wsky_ASTNode_delete(const wsky_ASTNode *node);



typedef struct {
  wsky_ASTNode_HEAD

  union {
    int64_t intValue;
    double floatValue;
    char *stringValue;
  } v;
} wsky_LiteralNode;

wsky_LiteralNode *wsky_LiteralNode_new(wsky_Token *token);
void wsky_LiteralNode_delete(wsky_LiteralNode *node);



typedef struct {
  wsky_ASTNode_HEAD

  /* NULL if unary operator */
  wsky_ASTNode *left;

  wsky_Operator operator;

  wsky_ASTNode *right;

} wsky_OperatorNode;

void wsky_OperatorNode_delete(wsky_OperatorNode *node);



#define wsky_ListNode_HEAD \
  wsky_ASTNode_HEAD

typedef struct {
  wsky_ListNode_HEAD

} wsky_ListNode;

void wsky_ListNode_delete(wsky_ListNode *node);



#endif /* !AST_H_ */
