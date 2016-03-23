#ifndef PARSER_H_
# define PARSER_H_

#include "ast.h"
#include "syntax_error.h"
#include "objects/program_file.h"

/**
 * @defgroup parser parser
 * The parser
 * @{
 */

typedef struct wsky_ParserResult_s wsky_ParserResult;

/**
 * The result returned by the parser
 */
struct wsky_ParserResult_s {

  /** false on error */
  bool success;

  /** Undefined on success */
  wsky_SyntaxError syntaxError;

  /** `NULL` on error */
  wsky_ASTNode *node;
};



wsky_ParserResult wsky_parse(wsky_TokenList *tokens);

/* For the REPL */
wsky_ParserResult wsky_parseLine(wsky_TokenList *tokens);

wsky_ParserResult wsky_parseTemplate(wsky_TokenList *tokens);

wsky_ParserResult wsky_parseString(const char *string);
wsky_ParserResult wsky_parseTemplateString(const char *string);
wsky_ParserResult wsky_parseFile(wsky_ProgramFile *file);

/**
 * @}
 */

#endif /* !PARSER_H_ */
