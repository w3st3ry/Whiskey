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

  wsky_ASTNodeType_UNARY_OPERATOR,
  wsky_ASTNodeType_BINARY_OPERATOR,

} wsky_ASTNodeType;



#define wsky_ASTNode_HEAD \
  wsky_ASTNodeType type;  \
  wsky_Token token;


typedef struct {
  wsky_ASTNode_HEAD
} wsky_ASTNode;

/**
 * Returns a malloc'd string.
 */
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

wsky_OperatorNode *wsky_OperatorNode_new(const wsky_Token *token,
					 wsky_ASTNode *left,
					 wsky_Operator operator,
					 wsky_ASTNode *right);

wsky_OperatorNode *wsky_OperatorNode_newUnary(const wsky_Token *token,
					      wsky_Operator operator,
					      wsky_ASTNode *right);



/* A linked list of ASTNode */
typedef struct wsky_ASTNodeList_s wsky_ASTNodeList;

struct wsky_ASTNodeList_s {
  wsky_ASTNode *node;
  wsky_ASTNodeList *next;
};

wsky_ASTNodeList *wsky_ASTNodeList_new(wsky_ASTNode *node,
				       wsky_ASTNodeList *next);

/* Returns the last node or NULL if the list is empty */
wsky_ASTNodeList *wsky_ASTNodeList_getLast(wsky_ASTNodeList *list);

/* Returns the last node or NULL if the list is empty */
wsky_ASTNode *wsky_ASTNodeList_getLastNode(wsky_ASTNodeList *list);

void wsky_ASTNodeList_add(wsky_ASTNodeList **listPointer,
			  wsky_ASTNodeList *new);

void wsky_ASTNodeList_addNode(wsky_ASTNodeList **listPointer,
			      wsky_ASTNode *node);

void wsky_ASTNodeList_delete(wsky_ASTNodeList *list);



#define wsky_ListNode_HEAD \
  wsky_ASTNode_HEAD

typedef struct {
  wsky_ListNode_HEAD

} wsky_ListNode;



#endif /* !AST_H_ */
