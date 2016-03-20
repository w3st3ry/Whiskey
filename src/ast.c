#include "ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "objects/str.h"
#include "memory.h"
#include "string_utils.h"

typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;
typedef wsky_Token Token;
typedef wsky_Position Position;



/** Forward declarations of some functions */
#define D(name)                                                         \
  typedef wsky_##name##Node name##Node;                                 \
                                                                        \
  static void name##Node_copy(const name##Node *src, name##Node *new);  \
  static void name##Node_free(name##Node *node);                        \
  static char *name##Node_toString(const name##Node *node);

D(Literal)
D(Identifier)
D(Html)
D(TpltPrint)
D(Operator)
D(Sequence)
D(Function)
D(Var)
D(Assignment)
D(Call)
D(MemberAccess)
D(Class)
D(ClassMember)
D(Import)
D(Export)
D(If)

#undef D



Node *wsky_ASTNode_copy(const Node *source) {

# define R(T)                                           \
  {                                                     \
    T##Node *new = wsky_safeMalloc(sizeof(T##Node));    \
    new->position = source->position;                   \
    new->type = source->type;                           \
    T##Node_copy((const T##Node *) source, new);        \
    return (Node *)new;                                 \
  }

# define CASE(type, name) case wsky_ASTNodeType_##type: R(name)

  switch (source->type) {

  case wsky_ASTNodeType_NULL:
  case wsky_ASTNodeType_BOOL:
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    R(Literal);

  case wsky_ASTNodeType_IDENTIFIER:
  case wsky_ASTNodeType_SELF:
  case wsky_ASTNodeType_SUPER:
  case wsky_ASTNodeType_SUPERCLASS:
    R(Identifier);

    CASE(HTML, Html);
    CASE(TPLT_PRINT, TpltPrint);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    R(Operator);

    CASE(SEQUENCE, Sequence);
    CASE(FUNCTION, Function);
    CASE(VAR, Var);
    CASE(ASSIGNMENT, Assignment);
    CASE(CALL, Call);
    CASE(MEMBER_ACCESS, MemberAccess);
    CASE(CLASS, Class);
    CASE(CLASS_MEMBER, ClassMember);
    CASE(IMPORT, Import);
    CASE(EXPORT, Export);
    CASE(IF, If);

  default:
    return NULL;
  }

# undef CASE
# undef R
}



bool wsky_ASTNode_isAssignable(const Node *node) {
  return (node->type == wsky_ASTNodeType_IDENTIFIER ||
          node->type == wsky_ASTNodeType_MEMBER_ACCESS);
}



char *wsky_ASTNode_toString(const Node *node) {

# define R(T)                                           \
  return T##Node_toString((const T##Node *) node);
# define CASE(type, name) case wsky_ASTNodeType_##type: R(name)

  switch (node->type) {

  case wsky_ASTNodeType_NULL:
  case wsky_ASTNodeType_BOOL:
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    R(Literal)

  case wsky_ASTNodeType_IDENTIFIER:
  case wsky_ASTNodeType_SELF:
  case wsky_ASTNodeType_SUPER:
  case wsky_ASTNodeType_SUPERCLASS:
    R(Identifier);

    CASE(HTML, Html);
    CASE(TPLT_PRINT, TpltPrint);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    R(Operator);

    CASE(SEQUENCE, Sequence);
    CASE(FUNCTION, Function);
    CASE(VAR, Var);
    CASE(ASSIGNMENT, Assignment);
    CASE(CALL, Call);
    CASE(MEMBER_ACCESS, MemberAccess);
    CASE(CLASS, Class);
    CASE(CLASS_MEMBER, ClassMember);
    CASE(IMPORT, Import);
    CASE(EXPORT, Export);
    CASE(IF, If);

  default:
    return wsky_strdup("Unknown node");
  }

# undef CASE
# undef R
}



void wsky_ASTNode_print(const Node *node, FILE *output) {
  char *s = wsky_ASTNode_toString(node);
  fprintf(output, "%s", s);
  wsky_free(s);
}



void wsky_ASTNode_delete(Node *node) {

# define R(T)                                   \
  T##Node_free((T##Node *) node); break;
# define CASE(type, name) case wsky_ASTNodeType_##type: R(name)

  switch (node->type) {

  case wsky_ASTNodeType_NULL:
  case wsky_ASTNodeType_BOOL:
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    R(Literal)

  case wsky_ASTNodeType_IDENTIFIER:
  case wsky_ASTNodeType_SELF:
  case wsky_ASTNodeType_SUPER:
  case wsky_ASTNodeType_SUPERCLASS:
    R(Identifier);

    CASE(HTML, Html);
    CASE(TPLT_PRINT, TpltPrint);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    R(Operator);

    CASE(SEQUENCE, Sequence);
    CASE(FUNCTION, Function);
    CASE(VAR, Var);
    CASE(ASSIGNMENT, Assignment);
    CASE(CALL, Call);
    CASE(MEMBER_ACCESS, MemberAccess);
    CASE(CLASS, Class);
    CASE(CLASS_MEMBER, ClassMember);
    CASE(IMPORT, Import);
    CASE(EXPORT, Export);
    CASE(IF, If);

  default:
    abort();
  }
  wsky_free(node);

# undef CASE
# undef R
}



LiteralNode *wsky_LiteralNode_new(const Token *token) {
  if (!wsky_Token_isLiteral(token))
    return NULL;

  LiteralNode *node = wsky_safeMalloc(sizeof(LiteralNode));
  if (!node)
    return NULL;

  node->position = token->begin;

  if (token->type == wsky_TokenType_KEYWORD) {
    wsky_Keyword keyword = token->v.keyword;

    if (keyword == wsky_Keyword_TRUE) {
      node->type = wsky_ASTNodeType_BOOL;
      node->v.boolValue = true;
    } else if (keyword == wsky_Keyword_FALSE) {
      node->type = wsky_ASTNodeType_BOOL;
      node->v.boolValue = false;
    } else if (keyword == wsky_Keyword_NULL) {
      node->type = wsky_ASTNodeType_NULL;
    } else {
      wsky_free(node);
      return NULL;
    }

  } else if (token->type == wsky_TokenType_INT) {
    node->type = wsky_ASTNodeType_INT;
    node->v.intValue = token->v.intValue;

  } else if (token->type == wsky_TokenType_FLOAT) {
    node->type = wsky_ASTNodeType_FLOAT;
    node->v.floatValue = token->v.floatValue;

  } else if (token->type == wsky_TokenType_STRING) {
    node->type = wsky_ASTNodeType_STRING;
    node->v.stringValue = wsky_strdup(token->v.stringValue);

  } else {
    wsky_free(node);
    return NULL;
  }

  return node;
}

void LiteralNode_copy(const LiteralNode *source, LiteralNode *new) {
  if (source->type == wsky_ASTNodeType_STRING) {
    new->v.stringValue = wsky_strdup(source->v.stringValue);
  } else {
    new->v = source->v;
  }
}

static void LiteralNode_free(LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    wsky_free(node->v.stringValue);
  }
}

static char *stringNodeToString(const LiteralNode *node) {
  return wsky_String_escapeCString(node->v.stringValue);
}

static char *intNodeToString(const LiteralNode *node) {
  return wsky_asprintf("%ld", (long) node->v.intValue);
}

static char *floatNodeToString(const LiteralNode *node) {
  wsky_Value value = wsky_Value_fromFloat(node->v.floatValue);
  wsky_ReturnValue stringRv = wsky_toString(value);
  assert(!stringRv.exception);
  assert(wsky_isString(stringRv.v));
  wsky_String *string = (wsky_String *)stringRv.v.v.objectValue;
  return wsky_strdup(string->string);
}

static char *LiteralNode_toString(const LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    return stringNodeToString(node);
  } else if (node->type == wsky_ASTNodeType_NULL) {
    return wsky_strdup("null");
  } else if (node->type == wsky_ASTNodeType_BOOL) {
    return wsky_strdup(node->v.boolValue ? "true" : "false");
  } else if (node->type == wsky_ASTNodeType_INT) {
    return intNodeToString(node);
  } else if (node->type == wsky_ASTNodeType_FLOAT) {
    return floatNodeToString(node);
  }
  return wsky_strdup("LiteralNode");
}



IdentifierNode *wsky_IdentifierNode_new(const char *name,
                                        wsky_ASTNodeType type,
                                        Position position) {
  IdentifierNode *node = wsky_safeMalloc(sizeof(IdentifierNode));
  node->type = type;
  node->position = position;
  node->name = name ? wsky_strdup(name) : NULL;
  return node;
}

IdentifierNode *wsky_IdentifierNode_newFromToken(const Token *token) {
  if (token->type != wsky_TokenType_IDENTIFIER)
    abort();

  return wsky_IdentifierNode_new(token->string,
                                 wsky_ASTNodeType_IDENTIFIER,
                                 token->begin);
}

void IdentifierNode_copy(const IdentifierNode *source, IdentifierNode *new) {
  new->name = source->name ? wsky_strdup(source->name) : NULL;
}

static void IdentifierNode_free(IdentifierNode *node) {
  wsky_free(node->name);
}

static const char *identifierToString(const IdentifierNode *node) {
  switch (node->type) {
  case wsky_ASTNodeType_IDENTIFIER:
    return node->name;
  case wsky_ASTNodeType_SELF:
    return "@";
  case wsky_ASTNodeType_SUPER:
    return "super";
  case wsky_ASTNodeType_SUPERCLASS:
    return "superclass";
  default:
    break;
  }
  abort();
}

static char *IdentifierNode_toString(const IdentifierNode *node) {
  return wsky_strdup(identifierToString(node));
}



HtmlNode *wsky_HtmlNode_new(const Token *token) {
  if (token->type != wsky_TokenType_HTML)
    return NULL;

  HtmlNode *node = wsky_safeMalloc(sizeof(HtmlNode));
  node->type = wsky_ASTNodeType_HTML;
  node->position = token->begin;
  node->content = wsky_strdup(token->string);
  return node;
}

void HtmlNode_copy(const HtmlNode *source, HtmlNode *new) {
  new->content = wsky_strdup(source->content);
}

static void HtmlNode_free(HtmlNode *node) {
  wsky_free(node->content);
}

static char *HtmlNode_toString(const HtmlNode *node) {
  return wsky_asprintf("HTML(%s)", node->content);
}

TpltPrintNode *wsky_TpltPrintNode_new(const Token *token,
                                      Node *child) {
  if (token->type != wsky_TokenType_WSKY_PRINT)
    return NULL;

  TpltPrintNode *node = wsky_safeMalloc(sizeof(TpltPrintNode));
  node->type = wsky_ASTNodeType_TPLT_PRINT;
  node->position = token->begin;
  node->child = child;
  return node;
}

void TpltPrintNode_copy(const TpltPrintNode *source, TpltPrintNode *new) {
  new->child = wsky_ASTNode_copy(source->child);
}

static void TpltPrintNode_free(TpltPrintNode *node) {
  wsky_ASTNode_delete(node->child);
}

static char *TpltPrintNode_toString(const TpltPrintNode *node) {
  char *childString =  wsky_ASTNode_toString(node->child);
  char *s = wsky_asprintf("TPLT_PRINT(%s)", childString);
  wsky_free(childString);
  return s;
}



OperatorNode *wsky_OperatorNode_new(const Token *token,
                                         Node *left,
                                         wsky_Operator operator,
                                         Node *right) {
  if (!left || !right || token->type != wsky_TokenType_OPERATOR)
    return NULL;

  OperatorNode *node = wsky_safeMalloc(sizeof(OperatorNode));
  node->type = wsky_ASTNodeType_BINARY_OPERATOR;
  node->position = token->begin;
  node->left = left;
  node->operator = operator;
  node->right = right;
  return node;
}

OperatorNode *wsky_OperatorNode_newUnary(const Token *token,
                                              wsky_Operator operator,
                                              Node *right) {
  if (!right || token->type != wsky_TokenType_OPERATOR)
    return NULL;

  OperatorNode *node = wsky_safeMalloc(sizeof(OperatorNode));
  node->type = wsky_ASTNodeType_UNARY_OPERATOR;
  node->position = token->begin;
  node->left = NULL;
  node->operator = operator;
  node->right = right;
  return node;
}

void OperatorNode_copy(const OperatorNode *source, OperatorNode *new) {
  new->operator = source->operator;
  new->right = wsky_ASTNode_copy(source->right);
  if (new->type == wsky_ASTNodeType_BINARY_OPERATOR) {
    new->left = wsky_ASTNode_copy(source->left);
  } else {
    new->left = NULL;
  }
}

static void OperatorNode_free(OperatorNode *node) {
  if (node->left)
    wsky_ASTNode_delete(node->left);
  wsky_ASTNode_delete(node->right);
}

static char *OperatorNode_toString(const OperatorNode *node) {
  char *left = NULL;
  if (node->left)
    left = wsky_ASTNode_toString(node->left);
  char *right = wsky_ASTNode_toString(node->right);

  const char *opString =  wsky_Operator_toString(node->operator);
  char *s;
  if (left) {
    s = wsky_asprintf("(%s %s %s)", left, opString, right);
    wsky_free(left);
  } else {
    s = wsky_asprintf("(%s %s)", opString, right);
  }
  wsky_free(right);
  return s;
}



NodeList *wsky_ASTNodeList_new(Node *node,
                               NodeList *next) {

  NodeList *list = wsky_safeMalloc(sizeof(NodeList));
  list->node = node;
  list->next = next;
  return list;
}

NodeList *wsky_ASTNodeList_copy(const NodeList *source) {
  if (!source)
    return NULL;
  Node *newNode = wsky_ASTNode_copy(source->node);
  NodeList *next = wsky_ASTNodeList_copy(source->next);
  NodeList *new = wsky_ASTNodeList_new(newNode, next);
  return new;
}

NodeList *wsky_ASTNodeList_getLast(NodeList *list) {
  if (!list) {
    return NULL;
  }
  if (!list->next) {
    return list;
  }
  return wsky_ASTNodeList_getLast(list->next);
}

Node *wsky_ASTNodeList_getLastNode(NodeList *list) {
  NodeList *last = wsky_ASTNodeList_getLast(list);
  if (!last)
    return NULL;
  return last->node;
}

void wsky_ASTNodeList_add(NodeList **listPointer,
                          NodeList *new) {
  if (!*listPointer) {
    *listPointer = new;
  } else {
    NodeList *last = wsky_ASTNodeList_getLast(*listPointer);
    last->next = new;
  }
}

void wsky_ASTNodeList_addNode(NodeList **listPointer,
                              Node *node) {
  NodeList *new = wsky_ASTNodeList_new(node, NULL);
  wsky_ASTNodeList_add(listPointer, new);
}

void wsky_ASTNodeList_delete(NodeList *list) {
  if (!list)
    return;
  wsky_ASTNodeList_delete(list->next);
  wsky_ASTNode_delete(list->node);
  wsky_free(list);
}

char *wsky_ASTNodeList_toString(NodeList *list, const char *separator) {
  char *s = NULL;
  size_t length = 0;
  while (list) {
    char *nodeString = wsky_ASTNode_toString(list->node);
    s = wsky_realloc(s, length + strlen(nodeString) + strlen(separator) + 1);
    if (!s) {
      abort();
    }
    s[length] = '\0';
    strcat(s, nodeString);
    if (list->next)
      strcat(s, separator);
    wsky_free(nodeString);
    length = strlen(s);
    list = list->next;
  }
  if (!s)
    s = wsky_strdup("");
  return s;
}



SequenceNode *wsky_SequenceNode_new(const wsky_Position *position,
                                    NodeList *children) {
  SequenceNode *node = wsky_safeMalloc(sizeof(SequenceNode));
  node->type = wsky_ASTNodeType_SEQUENCE;
  node->children = children;
  node->position = *position;
  node->program = false;
  return node;
}

void SequenceNode_copy(const SequenceNode *source, SequenceNode *new) {
  new->children = wsky_ASTNodeList_copy(source->children);
}

static void SequenceNode_free(SequenceNode *node) {
  wsky_ASTNodeList_delete(node->children);
}

static char *SequenceNode_toString(const SequenceNode *node) {
  char *list =  wsky_ASTNodeList_toString(node->children, "; ");
  if (node->program) {
    return list;
  }
  char *s = wsky_asprintf("(%s)", list);
  wsky_free(list);
  return s;
}



FunctionNode *wsky_FunctionNode_new(const Token *token,
                                         NodeList *parameters,
                                         NodeList *children) {
  FunctionNode *node = wsky_safeMalloc(sizeof(FunctionNode));
  node->type = wsky_ASTNodeType_FUNCTION;
  node->position = token->begin;
  node->children = children;
  node->parameters = parameters;
  node->name = NULL;
  return node;
}

void wsky_FunctionNode_setName(wsky_FunctionNode *node,
                               const char *newName) {
  wsky_free(node->name);
  node->name = wsky_strdup(newName);
}

void FunctionNode_copy(const FunctionNode *source, FunctionNode *new) {
  new->name = source->name ? wsky_strdup(source->name) : NULL;
  new->children = wsky_ASTNodeList_copy(source->children);
  new->parameters = wsky_ASTNodeList_copy(source->parameters);
}

static void FunctionNode_free(FunctionNode *node) {
  wsky_free(node->name);
  wsky_ASTNodeList_delete(node->children);
  wsky_ASTNodeList_delete(node->parameters);
}

static char *FunctionNode_toString(const FunctionNode *node) {
  char *childrenString =  wsky_ASTNodeList_toString(node->children, "; ");
  char *paramString =  wsky_ASTNodeList_toString(node->parameters, ", ");
  int hasParameters = node->parameters != NULL;
  char *s;
  if (hasParameters)
    s = wsky_asprintf("{%s: %s}", paramString, childrenString);
  else
    s = wsky_asprintf("{%s}", childrenString);
  wsky_free(paramString);
  wsky_free(childrenString);
  return s;
}



VarNode *wsky_VarNode_new(const Token *token,
                          const char *name,
                          Node *right) {
  VarNode *node = wsky_safeMalloc(sizeof(VarNode));
  node->type = wsky_ASTNodeType_VAR;
  node->position = token->begin;
  node->name = wsky_strdup(name);
  node->right = right;
  return node;
}

void VarNode_copy(const VarNode *source, VarNode *new) {
  new->name = wsky_strdup(source->name);
  if (source->right)
    new->right = wsky_ASTNode_copy(source->right);
}

static void VarNode_free(VarNode *node) {
  if (node->right)
    wsky_ASTNode_delete(node->right);
  wsky_free(node->name);
}

unsigned wsky_ASTNodeList_getCount(const NodeList *list) {
  if (!list) {
    return 0;
  }
  return wsky_ASTNodeList_getCount(list->next) + 1;
}

static char *VarNode_toString(const VarNode *node) {
  char *rightString = NULL;
  if (node->right) {
    rightString = wsky_ASTNode_toString(node->right);
  }
  char *s;
  if (node->right) {
    s = wsky_asprintf("var %s = %s", node->name, rightString);
    wsky_free(rightString);
  } else {
    s = wsky_asprintf("var %s", node->name);
  }
  return s;
}



AssignmentNode *wsky_AssignmentNode_new(const Token *token,
                                        Node *left,
                                        Node *right) {
  assert(wsky_ASTNode_isAssignable(left));
  AssignmentNode *node = wsky_safeMalloc(sizeof(AssignmentNode));
  node->type = wsky_ASTNodeType_ASSIGNMENT;
  node->position = token->begin;
  node->left = left;
  node->right = right;
  return node;
}

void AssignmentNode_copy(const AssignmentNode *source, AssignmentNode *new) {
  new->left = wsky_ASTNode_copy(source->left);
  new->right = wsky_ASTNode_copy(source->right);
}

static void AssignmentNode_free(AssignmentNode *node) {
  wsky_ASTNode_delete(node->left);
  wsky_ASTNode_delete(node->right);
}

static char *AssignmentNode_toString(const AssignmentNode *node) {
  char *leftString = wsky_ASTNode_toString(node->left);
  char *rightString = wsky_ASTNode_toString(node->right);
  char *s = wsky_asprintf("(%s = %s)", leftString, rightString);
  wsky_free(leftString);
  wsky_free(rightString);
  return s;
}



CallNode *wsky_CallNode_new(const Token *token,
                            Node *left,
                            NodeList *children) {
  CallNode *node = wsky_safeMalloc(sizeof(CallNode));
  node->type = wsky_ASTNodeType_CALL;
  node->position = token->begin;
  node->left = left;
  node->children = children;
  return node;
}

void CallNode_copy(const CallNode *source, CallNode *new) {
  new->left = wsky_ASTNode_copy(source->left);
  new->children = wsky_ASTNodeList_copy(source->children);
}

static void CallNode_free(CallNode *node) {
  wsky_ASTNodeList_delete(node->children);
  wsky_ASTNode_delete(node->left);
}

static char *CallNode_toString(const CallNode *node) {
  char *paramString =  wsky_ASTNodeList_toString(node->children, ", ");
  char *leftString =  wsky_ASTNode_toString(node->left);
  char *s = wsky_asprintf("%s(%s)", leftString, paramString);
  wsky_free(leftString);
  wsky_free(paramString);
  return s;
}



MemberAccessNode *wsky_MemberAccessNode_new(const Token *token,
                                            Node *left,
                                            const char *name) {
  MemberAccessNode *node = wsky_safeMalloc(sizeof(MemberAccessNode));
  node->type = wsky_ASTNodeType_MEMBER_ACCESS;
  node->position = token->begin;
  node->left = left;
  node->name = wsky_strdup(name);
  return node;
}

void MemberAccessNode_copy(const MemberAccessNode *source,
                           MemberAccessNode *new) {
  new->left = wsky_ASTNode_copy(source->left);
  new->name = wsky_strdup(source->name);
}

static void MemberAccessNode_free(MemberAccessNode *node) {
  wsky_free(node->name);
  wsky_ASTNode_delete(node->left);
}

static char *MemberAccessNode_toString(const MemberAccessNode *node) {
  char *leftString =  wsky_ASTNode_toString(node->left);
  char *s = wsky_asprintf("%s.%s", leftString, node->name);
  wsky_free(leftString);
  return s;
}



ClassNode *wsky_ClassNode_new(const Token *token,
                              const char *name,
                              Node *superclass,
                              NodeList *interfaces,
                              NodeList *children) {

  ClassNode *node = wsky_safeMalloc(sizeof(ClassNode));
  node->type = wsky_ASTNodeType_CLASS;
  node->position = token->begin;
  node->name = wsky_strdup(name);
  node->superclass = superclass;
  node->interfaces = interfaces;
  node->children = children;
  return node;
}

void ClassNode_copy(const ClassNode *source, ClassNode *new) {
  new->name = wsky_strdup(source->name);
  new->superclass = source->superclass ?
    wsky_ASTNode_copy(source->superclass) : NULL;
  new->interfaces = wsky_ASTNodeList_copy(source->interfaces);
  new->children = wsky_ASTNodeList_copy(source->children);
}

static void ClassNode_free(ClassNode *node) {
  wsky_free(node->name);
  if (node->superclass)
    wsky_ASTNode_delete(node->superclass);
  wsky_ASTNodeList_delete(node->children);
  wsky_ASTNodeList_delete(node->interfaces);
}

static char *superclassesToString(const ClassNode *node) {
  if (!node->superclass)
    return NULL;
  char *super = wsky_ASTNode_toString(node->superclass);
  if (!node->interfaces)
    return super;
  char *interfaces = wsky_ASTNodeList_toString(node->interfaces, ", ");
  char *s = wsky_asprintf("%s, %s", super, interfaces);
  free(interfaces);
  free(super);
  return s;
}

static char *classBeginToString(const ClassNode *node) {
  char *superclasses = superclassesToString(node);
  size_t length = strlen(node->name) + 10;
  if (superclasses)
    length += strlen(superclasses);
  char *s = wsky_safeMalloc(length);
  strcpy(s, "class ");
  strcat(s, node->name);
  if (superclasses) {
    strcat(s, ": ");
    strcat(s, superclasses);
  }
  wsky_free(superclasses);
  return s;
}

static char *membersToString(const ClassNode *node) {
  return wsky_ASTNodeList_toString(node->children, "; ");
}

static char *ClassNode_toString(const ClassNode *node) {
  char *begin = classBeginToString(node);
  char *members = membersToString(node);
  char *s = wsky_asprintf("%s (%s)", begin, members);
  wsky_free(begin);
  wsky_free(members);
  return s;
}



ClassMemberNode *wsky_ClassMemberNode_new(const Token *token,
                                          const char *name,
                                          wsky_MethodFlags flags,
                                          Node *right) {
  ClassMemberNode *node = wsky_safeMalloc(sizeof(ClassMemberNode));
  node->type = wsky_ASTNodeType_CLASS_MEMBER;
  node->position = token->begin;
  node->name = name ? wsky_strdup(name) : NULL;
  if (right) {
    const char *functionName = name;
    if (flags & wsky_MethodFlags_INIT)
      functionName = "<init>";
    assert(right->type == wsky_ASTNodeType_FUNCTION);
    wsky_FunctionNode *function = (wsky_FunctionNode *)right;
    wsky_FunctionNode_setName(function, functionName);
  }
  node->right = right;
  node->flags = flags;
  return node;
}

void ClassMemberNode_copy(const ClassMemberNode *source,
                          ClassMemberNode *new) {
  new->name = source->name ? wsky_strdup(source->name) : NULL;
  new->right = wsky_ASTNode_copy((Node *)new);
  new->flags = source->flags;
}

static void ClassMemberNode_free(ClassMemberNode *node) {
  wsky_free(node->name);
  if (node->right)
    wsky_ASTNode_delete(node->right);
}

static void addWord(char *dest, const char *source) {
  if (*dest)
    strcat(dest, " ");
  strcat(dest, source);
}

static const char *flagsToString(wsky_MethodFlags flags) {
  static char buffer[200];
  *buffer = '\0';
  if (!(flags & wsky_MethodFlags_PUBLIC))
    addWord(buffer, "private");
  if (flags & wsky_MethodFlags_GET)
    addWord(buffer, "get");
  if (flags & wsky_MethodFlags_SET)
    addWord(buffer, "set");
  if (flags & wsky_MethodFlags_INIT)
    addWord(buffer, "init");
  return buffer;
}

static char *ClassMemberNode_toString(const ClassMemberNode *node) {
  char *right = (node->right ?
                 wsky_ASTNode_toString(node->right) :
                 NULL);

  const char *flags = flagsToString(node->flags);

  size_t length = strlen(flags) + 1;
  if (node->name)
    length += strlen(node->name) + 1;
  if (right)
    length += strlen(right) + 1;

  char *s = wsky_safeMalloc(length + 1);
  strcpy(s, flags);
  if (node->name) {
    addWord(s, "@");
    strcat(s, node->name);
  }
  if (right)
    addWord(s, right);
  wsky_free(right);
  return s;
}



ImportNode *wsky_ImportNode_new(Position position,
                                unsigned level, const char *name) {
  ImportNode *node = wsky_safeMalloc(sizeof(ImportNode));
  node->type = wsky_ASTNodeType_IMPORT;
  node->position = position;
  node->name = wsky_strdup(name);
  node->level = level;
  return node;
}

void ImportNode_copy(const ImportNode *source, ImportNode *new) {
  new->level = source->level;
  new->name = wsky_strdup(source->name);
}

static void ImportNode_free(ImportNode *node) {
  wsky_free(node->name);
}

static char *ImportNode_toString(const ImportNode *node) {
  // TODO
  return wsky_strdup("import");
}



ExportNode *wsky_ExportNode_new(Position position,
                                const char *name, Node *right) {
  ExportNode *node = wsky_safeMalloc(sizeof(ExportNode));
  node->type = wsky_ASTNodeType_EXPORT;
  node->position = position;
  node->name = wsky_strdup(name);
  node->right = right;
  return node;
}

void ExportNode_copy(const ExportNode *source, ExportNode *new) {
  new->name = wsky_strdup(source->name);
  new->right = source->right ? wsky_ASTNode_copy(source->right) : NULL;
}

static void ExportNode_free(ExportNode *node) {
  wsky_free(node->name);
  if (node->right)
    wsky_ASTNode_delete(node->right);
}

static char *ExportNode_toString(const ExportNode *node) {
  // TODO
  return wsky_strdup("export");
}



IfNode *wsky_IfNode_new(Position position,
                             NodeList *tests,
                             NodeList *expressions,
                             Node *elseNode) {
  assert(tests);
  assert(expressions);
  IfNode *node = wsky_safeMalloc(sizeof(IfNode));
  node->type = wsky_ASTNodeType_IF;
  node->position = position;
  node->tests = tests;
  node->expressions = expressions;
  node->elseNode = elseNode;
  return node;
}

void IfNode_copy(const IfNode *source, IfNode *new) {
  new->tests = wsky_ASTNodeList_copy(source->tests);
  new->expressions = wsky_ASTNodeList_copy(source->expressions);
  new->elseNode = (source->elseNode ?
                   wsky_ASTNode_copy(source->elseNode) :
                   NULL);
}

static void IfNode_free(IfNode *node) {
  wsky_ASTNodeList_delete(node->tests);
  wsky_ASTNodeList_delete(node->expressions);
  if (node->elseNode)
    wsky_ASTNode_delete(node->elseNode);
}

static char *ifToString(const Node *test, const Node *expression) {
  char *testString = wsky_ASTNode_toString(test);
  char *exprString = wsky_ASTNode_toString(expression);
  char *s = wsky_safeMalloc(strlen(testString) + strlen(exprString) + 8);
  sprintf(s, "if %s: %s", testString, exprString);
  wsky_free(testString);
  wsky_free(exprString);
  return s;
}

static char *IfNode_toString(const IfNode *node) {
  char *s = ifToString(node->tests->node, node->expressions->node);

  NodeList *tests = node->tests->next;
  NodeList *expressions = node->expressions->next;
  while (tests) {
    assert(expressions);
    char *expression = ifToString(tests->node, expressions->node);
    char *new = wsky_safeMalloc(strlen(s) + strlen(expression) + 8);
    sprintf(new, "%s else %s", s, expression);
    wsky_free(expression);
    wsky_free(s);
    s = new;
    tests = tests->next;
    expressions = expressions->next;
  }

  if (node->elseNode) {
    char *expression = wsky_ASTNode_toString(node->elseNode);
    char *new = wsky_safeMalloc(strlen(s) + strlen(expression) + 8);
    sprintf(new, "%s else: %s", s, expression);
    wsky_free(expression);
    wsky_free(s);
    s = new;
  }

  return s;
}
