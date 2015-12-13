#ifndef AST_H_
# define AST_H_

/* Abstract Syntax Tree */

#include "token.h"

typedef enum {

  /* Literals */
  wsky_ASTNodeType_INT,
  wsky_ASTNodeType_FLOAT,
  wsky_ASTNodeType_STRING,
  wsky_ASTNodeType_ARRAY,

  wsky_ASTNodeType_IDENTIFIER,

  wsky_ASTNodeType_PARENS,

  wsky_ASTNodeType_FUNCTION,

  wsky_ASTNodeType_ASSIGN,

  wsky_ASTNodeType_UNARY_OPERATION,
  wsky_ASTNodeType_BINARY_OPERATION,

} wsky_ASTNodeType;



#define wsky_ASTNode_HEAD \
  wsky_ASTNodeType type;  \
  wsky_Token token;


typedef struct {
  wsky_ASTNode_HEAD
} wsky_ASTNode;


char *wsky_ASTNode_toString(const wsky_ASTNode *node);
void wsky_ASTNode_print(const wsky_ASTNode *node, FILE *output);
void wsky_ASTNode_delete(wsky_ASTNode *node);



typedef struct {
  wsky_ASTNode_HEAD

  union {
    int64_t intValue;
    double floatValue;
    char *stringValue;
  } v;
} wsky_LiteralNode;

wsky_LiteralNode *wsky_LiteralNode_new(const wsky_Token *token);



typedef struct {
  wsky_ASTNode_HEAD

  char *name;
} wsky_IdentifierNode;

wsky_IdentifierNode *wsky_IdentifierNode_new(const wsky_Token *token);



typedef struct {
  wsky_ASTNode_HEAD

  /* NULL if unary operator */
  wsky_ASTNode *left;

  wsky_Operator operator;

  wsky_ASTNode *right;

} wsky_OperatorNode;



#define wsky_ListNode_HEAD \
  wsky_ASTNode_HEAD

typedef struct {
  wsky_ListNode_HEAD

} wsky_ListNode;



#endif /* !AST_H_ */
