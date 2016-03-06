/* Included from parser.c */

static ParserResult parseSuperclasses(TokenList **listPointer,
                                      NodeList **superclassesPointer) {
  *superclassesPointer = NULL;

  while (*listPointer) {
    ParserResult pr = parseTerm(listPointer);
    if (!pr.success) {
      wsky_ASTNodeList_delete(*superclassesPointer);
      return pr;
    }

    wsky_ASTNodeList_addNode(superclassesPointer, pr.node);

    Token *commaToken = tryToReadOperator(listPointer, OP(COMMA));
    if (!commaToken)
      break;
  }
  return ParserResult_NULL;
}



static ParserResult expectFunction(TokenList **listPointer,
                                   Position lastPosition) {
  if (!*listPointer)
    return createError("Expected function", lastPosition);

  ParserResult pr = parseFunction(listPointer);
  if (!pr.success)
    return pr;
  if (!pr.node)
    return createError("Expected function", lastPosition);
  return pr;
}


static bool isClassKeyword(const char *string) {
  const char *keywords[] = {
    "private", "get", "set", "init",
    NULL,
  };

  for (int i = 0; keywords[i]; i++) {
    if (strcmp(keywords[i], string) == 0)
      return true;
  }
  return false;
}


static ParserResult parseClassKeyword(TokenList **listPointer,
                                      Token **tokenPointer) {
  *tokenPointer = NULL;
  Token *token = tryToReadIdentifier(listPointer);
  if (!token)
    return ParserResult_NULL;

  if (!isClassKeyword(token->string))
    return createError("Unknown class keyword", token->begin);
  *tokenPointer = token;
  return ParserResult_NULL;
}


static ParserResult tryToReadClassKeyword(TokenList **listPointer,
                                          const char *kw,
                                          Token **tokenPointer) {
  TokenList *begin = *listPointer;
  ParserResult pr = parseClassKeyword(listPointer, tokenPointer);
  if (!pr.success)
    return pr;

  if (!*tokenPointer || strcmp((*tokenPointer)->string, kw)) {
    *tokenPointer = NULL;
    *listPointer = begin;
  }
  return ParserResult_NULL;
}


static ParserResult parseInit(TokenList **listPointer) {
  Token *init;
  ParserResult pr = tryToReadClassKeyword(listPointer, "init", &init);
  if (!pr.success)
    return pr;
  if (!init)
    return ParserResult_NULL;

  pr = expectFunction(listPointer, init->end);
  if (!pr.success)
    return pr;

  wsky_MethodFlags flags = wsky_MethodFlags_INIT | wsky_MethodFlags_PUBLIC;
  wsky_ClassMemberNode *node;
  node = wsky_ClassMemberNode_new(init, NULL,
                                  flags,
                                  pr.node);
  return createNodeResult((Node *)node);
}

static Token *tryToReadAtName(TokenList **listPointer) {
  Token *at = tryToReadOperator(listPointer, OP(AT));
  if (!at)
    return NULL;
  return (tryToReadIdentifier(listPointer));
}

static ParserResult parseFlags(TokenList **listPointer,
                               wsky_MethodFlags *flagsPointer,
                               Token **lastTokenPointer) {
  wsky_MethodFlags flags = wsky_MethodFlags_DEFAULT;
  Token *token;
  ParserResult pr;
  pr = tryToReadClassKeyword(listPointer, "private", &token);
  if (!pr.success)
    return pr;
  if (!token)
    flags |= wsky_MethodFlags_PUBLIC;
  *flagsPointer = flags;
  *lastTokenPointer = token;
  return ParserResult_NULL;
}


static unsigned getParameterCount(wsky_FunctionNode *function) {
  return wsky_ASTNodeList_getCount(function->parameters);
}


static ParserResult parseGetter(TokenList **listPointer,
                                wsky_MethodFlags flags) {
  Token *get;
  ParserResult pr = tryToReadClassKeyword(listPointer, "get", &get);
  if (!pr.success)
    return pr;
  if (!get)
    return ParserResult_NULL;

  Token *name = tryToReadAtName(listPointer);
  if (!name)
    return createError("Expected getter name (with an '@')", get->end);

  pr = parseFunction(listPointer);
  if (!pr.success)
    return pr;
  if (pr.node) {
    if (getParameterCount((wsky_FunctionNode *)pr.node) != 0) {
      wsky_ASTNode_delete(pr.node);
      return createError("A getter cannot have any parameter", get->end);
    }
  }

  flags |= wsky_MethodFlags_GET;
  wsky_ClassMemberNode *node;
  node = wsky_ClassMemberNode_new(get, name->string, flags, pr.node);
  return createNodeResult((Node *)node);
}

static ParserResult parseSetter(TokenList **listPointer,
                                wsky_MethodFlags flags) {
  Token *set;
  ParserResult pr = tryToReadClassKeyword(listPointer, "set", &set);
  if (!pr.success)
    return pr;
  if (!set)
    return ParserResult_NULL;

  Token *name = tryToReadAtName(listPointer);
  if (!name)
    return createError("Expected setter name (with an '@')", set->end);

  pr = parseFunction(listPointer);
  if (!pr.success)
    return pr;
  if (pr.node) {
    if (getParameterCount((wsky_FunctionNode *)pr.node) != 1) {
      wsky_ASTNode_delete(pr.node);
      return createError("A setter must have one parameter", set->end);
    }
  }

  flags |= wsky_MethodFlags_SET;
  wsky_ClassMemberNode *node;
  node = wsky_ClassMemberNode_new(set, name->string, flags, pr.node);
  return createNodeResult((Node *)node);
}

static ParserResult parseMethod(TokenList **listPointer,
                                wsky_MethodFlags flags,
                                Token *lastFlagToken) {
  Token *name = tryToReadAtName(listPointer);
  if (!name) {
    if (lastFlagToken)
      return createError("Expected method name (with an '@')",
                         lastFlagToken->end);
    return ParserResult_NULL;
  }

  ParserResult pr = expectFunction(listPointer, name->end);
  if (!pr.success)
    return pr;

  wsky_ClassMemberNode *node;
  node = wsky_ClassMemberNode_new(name, name->string, flags, pr.node);
  return createNodeResult((Node *)node);
}

static ParserResult parseClassMember(TokenList **listPointer) {
  ParserResult pr;

  pr = parseInit(listPointer);
  if (!pr.success || pr.node)
    return pr;

  wsky_MethodFlags flags;
  Token *flagsToken;
  pr = parseFlags(listPointer, &flags, &flagsToken);
  if (!pr.success)
    return pr;

  pr = parseGetter(listPointer, flags);
  if (!pr.success || pr.node)
    return pr;

  pr = parseSetter(listPointer, flags);
  if (!pr.success || pr.node)
    return pr;

  return parseMethod(listPointer, flags, flagsToken);
}

static inline bool isConstructor(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_INIT;
}

static inline bool isSetter(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_SET;
}

static inline bool isGetter(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_GET;
}

static inline bool isMethod(wsky_MethodFlags flags) {
  return (!isConstructor(flags) &&
          !isSetter(flags) &&
          !isGetter(flags));
}

static bool hasConstructor(const NodeList *nodeList) {
  if (!nodeList)
    return false;
  wsky_ClassMemberNode *node = (wsky_ClassMemberNode *)nodeList->node;
  if (isConstructor(node->flags))
    return true;
  return hasConstructor(nodeList->next);
}

static bool hasSetter(const NodeList *nodeList, const char *name) {
  if (!nodeList)
    return false;
  wsky_ClassMemberNode *node = (wsky_ClassMemberNode *)nodeList->node;
  if (isSetter(node->flags) && strcmp(name, node->name) == 0)
      return true;
  return hasSetter(nodeList->next, name);
}

static bool hasGetter(const NodeList *nodeList, const char *name) {
  if (!nodeList)
    return false;
  wsky_ClassMemberNode *node = (wsky_ClassMemberNode *)nodeList->node;
  if (isGetter(node->flags) && strcmp(name, node->name) == 0)
      return true;
  return hasGetter(nodeList->next, name);
}

static bool hasMethod(const NodeList *nodeList, const char *name) {
  if (!nodeList)
    return false;
  wsky_ClassMemberNode *node = (wsky_ClassMemberNode *)nodeList->node;
  if (isMethod(node->flags) && strcmp(name, node->name) == 0)
      return true;
  return hasMethod(nodeList->next, name);
}


static ParserResult parseClassMemberCheck(TokenList **listPointer,
                                          const NodeList *nodeList) {
  ParserResult pr = parseClassMember(listPointer);
  if (!pr.success || !pr.node)
    return pr;

  wsky_ClassMemberNode *node = (wsky_ClassMemberNode *)pr.node;

  Position position = node->position;
  if (isConstructor(node->flags) && hasConstructor(nodeList)) {
    wsky_ASTNode_delete(pr.node);
    return createError("Constructor redefinition", position);
  }
  if (isGetter(node->flags) || isMethod(node->flags)) {
    if (hasGetter(nodeList, node->name) || hasMethod(nodeList, node->name)) {
      wsky_ASTNode_delete(pr.node);
      return createError("Getter or method redefinition", position);
    }
  }
  if (isSetter(node->flags) && hasSetter(nodeList, node->name)) {
    wsky_ASTNode_delete(pr.node);
    return createError("Setter redefinition", position);
  }
  return createNodeResult(pr.node);
}


static ParserResult parseClassMembers(TokenList **listPointer,
                                      NodeList **nodeListPointer) {
  *nodeListPointer = NULL;
  while (*listPointer) {
    ParserResult pr = parseClassMemberCheck(listPointer, *nodeListPointer);
    if (!pr.success)
      return pr;
    if (!pr.node)
      break;

    wsky_ASTNodeList_addNode(nodeListPointer, pr.node);

    Token *sepToken = tryToReadOperator(listPointer, OP(SEMICOLON));
    if (!sepToken)
      break;
  }
  return ParserResult_NULL;
}


static NodeList *getInterfaces(const NodeList *superclasses) {
  if (!superclasses || !superclasses->next)
    return NULL;
  return wsky_ASTNodeList_copy(superclasses->next);
}

static ParserResult parseClass(TokenList **listPointer) {
  Token *classToken = tryToReadKeyword(listPointer, wsky_Keyword_CLASS);
  if (!classToken)
    return ParserResult_NULL;
  if (!*listPointer)
    return createError("Expected class name", classToken->end);

  const Token *name = tryToReadIdentifier(listPointer);
  if (!name)
    return createError("Expected class name", classToken->end);

  NodeList *superclasses = NULL;
  Token *colon = tryToReadOperator(listPointer, OP(COLON));
  if (colon) {
    ParserResult pr = parseSuperclasses(listPointer, &superclasses);
    if (!pr.success)
      return pr;
  }

  Token *leftParen = tryToReadOperator(listPointer, OP(LEFT_PAREN));
  if (!leftParen) {
    wsky_ASTNodeList_delete(superclasses);
    Position pos = name->begin;
    return createError("Expected '(', superclass or interface", pos);
  }

  NodeList *children = NULL;
  ParserResult pr = parseClassMembers(listPointer, &children);
  if (!pr.success) {
    wsky_ASTNodeList_delete(superclasses);
    wsky_ASTNodeList_delete(children);
    return pr;
  }

  Token *rightParen = tryToReadOperator(listPointer, OP(RIGHT_PAREN));
  if (!rightParen) {
    wsky_ASTNodeList_delete(superclasses);
    wsky_ASTNodeList_delete(children);
    return createError("Expected ')'", leftParen->end);
  }

  Node *superclass = superclasses ?
    wsky_ASTNode_copy(superclasses->node) : NULL;

  NodeList *interfaces = getInterfaces(superclasses);
  wsky_ASTNodeList_delete(superclasses);

  Node *classNode = (Node *)wsky_ClassNode_new(classToken, name->string,
                                               superclass,
                                               interfaces,
                                               children);
  return createNodeResult(classNode);
}
