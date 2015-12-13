#ifndef PARSER_H_
# define PARSER_H_

#include "ast.h"
#include "syntax_error.h"

typedef struct wsky_ParserResult_s wsky_ParserResult;

struct wsky_ParserResult_s {
  bool success;

  /** Undefined on success */
  wsky_SyntaxError syntaxError;

  /** `NULL` on error */
  wsky_ASTNode *node;
};



wsky_ParserResult wsky_parse(wsky_TokenList *tokens);

#endif /* !PARSER_H_ */
