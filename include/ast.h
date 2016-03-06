#ifndef AST_H_
# define AST_H_

# include "position.h"
# include "token.h"
# include "method_def.h"

/**
 * @defgroup ast ast
 * Abstract Syntax Tree
 * @addtogroup ast
 * @{
 */

/**
 * An enumeration of the types of the nodes
 */
typedef enum {
  /* Templates only */
  wsky_ASTNodeType_HTML,
  wsky_ASTNodeType_TPLT_PRINT,

  /* Literals */
  wsky_ASTNodeType_NULL,
  wsky_ASTNodeType_BOOL,
  wsky_ASTNodeType_INT,
  wsky_ASTNodeType_FLOAT,
  wsky_ASTNodeType_STRING,
  wsky_ASTNodeType_ARRAY,

  wsky_ASTNodeType_IDENTIFIER,
  /* @ */
  wsky_ASTNodeType_SELF,
  wsky_ASTNodeType_SUPER,
  wsky_ASTNodeType_SUPERCLASS,

  /* Variable declaration */
  wsky_ASTNodeType_VAR,

  wsky_ASTNodeType_ASSIGNMENT,

  /* Parentheses */
  wsky_ASTNodeType_SEQUENCE,

  /* Function definition */
  wsky_ASTNodeType_FUNCTION,

  /* Function call */
  wsky_ASTNodeType_CALL,

  wsky_ASTNodeType_UNARY_OPERATOR,
  wsky_ASTNodeType_BINARY_OPERATOR,

  wsky_ASTNodeType_MEMBER_ACCESS,

  /* Class definition */
  wsky_ASTNodeType_CLASS,

  wsky_ASTNodeType_CLASS_MEMBER,

  wsky_ASTNodeType_IMPORT,

  wsky_ASTNodeType_EXPORT,

  wsky_ASTNodeType_IF,

} wsky_ASTNodeType;



# define wsky_ASTNode_HEAD                      \
                                                \
  /** The node type */                          \
  wsky_ASTNodeType type;                        \
                                                \
  /** The position of the node in its file */   \
  wsky_Position position;

/**
 * @defgroup ASTNode ASTNode
 * @{
 */

/**
 * A node of the Abstract Syntax Tree.
 */
typedef struct {
  wsky_ASTNode_HEAD
} wsky_ASTNode;


/**
 * Returns a deep copy of the given node.
 */
wsky_ASTNode *wsky_ASTNode_copy(const wsky_ASTNode *source);

/**
 * Return true if the node is assignable with `=`.
 */
bool wsky_ASTNode_isAssignable(const wsky_ASTNode *node);

/**
 * Returns a malloc'd string.
 */
char *wsky_ASTNode_toString(const wsky_ASTNode *node);

/**
 * Prints the node to the given file.
 */
void wsky_ASTNode_print(const wsky_ASTNode *node, FILE *output);

/**
 * Deletes the node and its children.
 */
void wsky_ASTNode_delete(wsky_ASTNode *node);

/**
 * @}
 */



/**
 * A litteral integer, float, boolean, null or string node
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** An union of the values */
  union {
    /** If type == INT */
    wsky_int intValue;

    /** If type == FLOAT */
    wsky_float floatValue;

    /** If type == STRING */
    char *stringValue;

    /** If type == BOOL */
    bool boolValue;
  } v;

} wsky_LiteralNode;

/** Creates a new wsky_LiteralNode */
wsky_LiteralNode *wsky_LiteralNode_new(const wsky_Token *token);


/**
 * An identifier node
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The identifier or NULL */
  char *name;
} wsky_IdentifierNode;

/** Creates a new wsky_IdentifierNode from a wsky_Token */
wsky_IdentifierNode *wsky_IdentifierNode_newFromToken(const wsky_Token *t);

/** Creates a new wsky_IdentifierNode  */
wsky_IdentifierNode *wsky_IdentifierNode_new(const char *name,
                                             wsky_ASTNodeType type,
                                             wsky_Position position);


/**
 * An HTML node
 * Template-only.
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The HTML source code */
  char *content;
} wsky_HtmlNode;

wsky_HtmlNode *wsky_HtmlNode_new(const wsky_Token *token);



/**
 * A 'print' node (delimited with `<=` and `>`)
 * Template-only.
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The child node */
  wsky_ASTNode *child;

} wsky_TpltPrintNode;

wsky_TpltPrintNode *wsky_TpltPrintNode_new(const wsky_Token *token,
                                           wsky_ASTNode *child);


/**
 * An operator node
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The left node - NULL if unary operator */
  wsky_ASTNode *left;

  /** The operator */
  wsky_Operator operator;

  /** The right node */
  wsky_ASTNode *right;

} wsky_OperatorNode;

/** Creates a binary operator */
wsky_OperatorNode *wsky_OperatorNode_new(const wsky_Token *token,
                                         wsky_ASTNode *left,
                                         wsky_Operator operator,
                                         wsky_ASTNode *right);

/** Creates an unary operator */
wsky_OperatorNode *wsky_OperatorNode_newUnary(const wsky_Token *token,
                                              wsky_Operator operator,
                                              wsky_ASTNode *right);



/**
 * A linked list of ASTNode
 */
typedef struct wsky_ASTNodeList_s {

  /** The node of this element (the first node of the list) */
  wsky_ASTNode *node;

  /** The next element or null */
  struct wsky_ASTNodeList_s *next;

} wsky_ASTNodeList;

/**
 * Creates a new node list.
 */
wsky_ASTNodeList *wsky_ASTNodeList_new(wsky_ASTNode *node,
                                       wsky_ASTNodeList *next);

/**
 * Performs a deep copy of the given node list.
 */
wsky_ASTNodeList *wsky_ASTNodeList_copy(const wsky_ASTNodeList *source);

/**
 * Returns the last element or NULL if the list is empty.
 */
wsky_ASTNodeList *wsky_ASTNodeList_getLast(wsky_ASTNodeList *list);

/**
 * Returns the last node or NULL if the list is empty.
 */
wsky_ASTNode *wsky_ASTNodeList_getLastNode(wsky_ASTNodeList *list);

/**
 * Adds an element to the list.
 */
void wsky_ASTNodeList_add(wsky_ASTNodeList **listPointer,
                          wsky_ASTNodeList *new);

/**
 * Adds a node to the list.
 */
void wsky_ASTNodeList_addNode(wsky_ASTNodeList **listPointer,
                              wsky_ASTNode *node);

/**
 * Delete the list and its nodes.
 */
void wsky_ASTNodeList_delete(wsky_ASTNodeList *list);

/**
 * Returns the node count in the given list.
 */
unsigned wsky_ASTNodeList_getCount(const wsky_ASTNodeList *list);

/**
 * Returns a malloc'd string.
 *
 * @param list The list
 * @param separator A null-terminated string used too
 * separate the nodes (usually a comma or a semicolon)
 */
char *wsky_ASTNodeList_toString(wsky_ASTNodeList *list,
                                const char *separator);



# define wsky_ListNode_HEAD                     \
  wsky_ASTNode_HEAD                             \
                                                \
  /** The children */                           \
  wsky_ASTNodeList *children;

/**
 * An sequence node (parentheses)
 */
typedef struct {
  wsky_ListNode_HEAD

  /** `true` if this node is the root of a program. */
  bool program;
} wsky_SequenceNode;



wsky_SequenceNode *wsky_SequenceNode_new(const wsky_Position *position,
                                         wsky_ASTNodeList *children);



/**
 * A function node (a definition, not a call)
 */
typedef struct {
  wsky_ListNode_HEAD

  /** A list of the parameters */
  wsky_ASTNodeList *parameters;

  /** The name or NULL */
  char *name;

} wsky_FunctionNode;

/** Creates a function node */
wsky_FunctionNode *wsky_FunctionNode_new(const wsky_Token *token,
                                         wsky_ASTNodeList *parameters,
                                         wsky_ASTNodeList *children);

void wsky_FunctionNode_setName(wsky_FunctionNode *node, const char *newName);


/**
 * A variable declaration node
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The variable name */
  char *name;

  /** The right node (the value to assign to the variable) or NULL */
  wsky_ASTNode *right;

} wsky_VarNode;

wsky_VarNode *wsky_VarNode_new(const wsky_Token *token,
                               const char *name,
                               wsky_ASTNode *right);


/**
 * An assignment node
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The left node */
  wsky_ASTNode *left;

  /** The right node */
  wsky_ASTNode *right;

} wsky_AssignmentNode;

wsky_AssignmentNode *wsky_AssignmentNode_new(const wsky_Token *token,
                                             wsky_ASTNode *left,
                                             wsky_ASTNode *right);


/**
 * A function call node
 */
typedef struct {
  wsky_ListNode_HEAD

  /** The node of the function to call */
  wsky_ASTNode *left;

} wsky_CallNode;

wsky_CallNode *wsky_CallNode_new(const wsky_Token *token,
                                 wsky_ASTNode *left,
                                 wsky_ASTNodeList *children);


/**
 * A member access node (the `.` operator)
 */
typedef struct {
  wsky_ASTNode_HEAD

  /** The node of the object */
  wsky_ASTNode *left;

  /** The member name */
  char *name;

} wsky_MemberAccessNode;

wsky_MemberAccessNode *wsky_MemberAccessNode_new(const wsky_Token *token,
                                                 wsky_ASTNode *left,
                                                 const char *name);


/** A class definition */
typedef struct {
  wsky_ListNode_HEAD

  /** The class name */
  char *name;

  /** The superclass or NULL */
  wsky_ASTNode *superclass;

  /** The implemented interfaces */
  wsky_ASTNodeList *interfaces;

} wsky_ClassNode;

wsky_ClassNode *wsky_ClassNode_new(const wsky_Token *token,
                                   const char *name,
                                   wsky_ASTNode *superclass,
                                   wsky_ASTNodeList *interfaces,
                                   wsky_ASTNodeList *children);


/** A class member */
typedef struct {
  wsky_ASTNode_HEAD

  /** The member name or NULL if constructor */
  char *name;

  wsky_MethodFlags flags;

  /** The right node or NULL */
  wsky_ASTNode *right;
} wsky_ClassMemberNode;

wsky_ClassMemberNode *wsky_ClassMemberNode_new(const wsky_Token *token,
                                               const char *name,
                                               wsky_MethodFlags flags,
                                               wsky_ASTNode *right);



/** An `import` statement */
typedef struct {
  wsky_ASTNode_HEAD

  /** 0 to import a top-level module, 1 to import from
   * the current directory, 2 to import from the parent directory, ... */
  unsigned level;

  char *name;
} wsky_ImportNode;

/**
 * @param position The position of the node
 * @param level 0 to import a top-level module, 1 to import from
 * the current directory, 2 to import from the parent directory, ...
 * @param name The name of the module to import
 */
wsky_ImportNode *wsky_ImportNode_new(wsky_Position position,
                                     unsigned level, const char *name);



/** An `export` statement */
typedef struct {
  wsky_ASTNode_HEAD

  char *name;

  /* The value or NULL */
  wsky_ASTNode *right;
} wsky_ExportNode;

/**
 * @param position The position of the keyword
 * @param name The name of the variable to declare and to export
 * @param right The right node or NULL
 */
wsky_ExportNode *wsky_ExportNode_new(wsky_Position position,
                                     const char *name,
                                     wsky_ASTNode *right);



/** An `if` statement */
typedef struct wsky_IfNode_s {
  wsky_ASTNode_HEAD

  wsky_ASTNodeList *tests;

  wsky_ASTNodeList *expressions;

  /** An `else` node or NULL */
  wsky_ASTNode *elseNode;
} wsky_IfNode;

wsky_IfNode *wsky_IfNode_new(wsky_Position position,
                             wsky_ASTNodeList *tests,
                             wsky_ASTNodeList *expressions,
                             wsky_ASTNode *elseNode);

/**
 * @}
 */

#endif /* !AST_H_ */
