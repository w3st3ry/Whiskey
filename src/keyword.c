#include <string.h>
#include "whiskey.h"

typedef wsky_Keyword Keyword;

typedef struct {
  Keyword keyword;
  const char *name;
} MapEntry;

static const MapEntry KEYWORD_MAP[] = {
  {wsky_Keyword_AND, "and"},
  {wsky_Keyword_BREAK, "break"},
  {wsky_Keyword_CASE, "case"},
  {wsky_Keyword_CLASS, "class"},
  {wsky_Keyword_CONST, "const"},
  {wsky_Keyword_ELSE, "else"},
  {wsky_Keyword_EXCEPT, "except"},
  {wsky_Keyword_EXPORT, "export"},
  {wsky_Keyword_FALSE, "false"},
  {wsky_Keyword_FINAL, "final"},
  {wsky_Keyword_FINALLY, "finally"},
  {wsky_Keyword_FOR, "for"},
  {wsky_Keyword_IF, "if"},
  {wsky_Keyword_IMPORT, "import"},
  {wsky_Keyword_IN, "in"},
  {wsky_Keyword_INTERFACE, "interface"},
  {wsky_Keyword_NOT, "not"},
  {wsky_Keyword_NULL, "null"},
  {wsky_Keyword_OR, "or"},
  {wsky_Keyword_RETURN, "return"},
  {wsky_Keyword_SUPER, "super"},
  {wsky_Keyword_SUPERCLASS, "superclass"},
  {wsky_Keyword_SWITCH, "switch"},
  {wsky_Keyword_TRUE, "true"},
  {wsky_Keyword_TRY, "try"},
  {wsky_Keyword_VAR, "var"},
  {wsky_Keyword_WHILE, "while"},

  // This one is not a valid entry, it just marks the end.
  {wsky_Keyword_CLASS, NULL},
};



bool wsky_Keyword_parse(const char *string, wsky_Keyword *keywordPointer) {
  int i;
  for (i = 0;; i++) {
    const MapEntry *entry = KEYWORD_MAP + i;
    if (!entry->name) {
      break;
    }
    if (strcmp(entry->name, string) == 0) {
      *keywordPointer = entry->keyword;
      return false;
    }
  }
  return true;
}
