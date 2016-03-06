#include "string_reader.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "gc.h"
#include "string_utils.h"


typedef wsky_Token Token;
typedef wsky_Position Position;
typedef wsky_StringReader StringReader;


StringReader wsky_StringReader_create(wsky_ProgramFile *file,
                                      const char *string) {
  assert(file);
  assert(string);

  Position pos = {
    .index = 0,
    .line = 1,
    .column = 0,
    .file = file,
  };
  StringReader reader = {
    .file = file,
    .string = string,
    .position = pos,
  };
  return reader;
}

void wsky_StringReader_free(StringReader *reader) {
  (void) reader;
}



bool wsky_StringReader_hasMore(const StringReader *reader) {
  return reader->position.index < (int)strlen(reader->string);
}

char wsky_StringReader_next(StringReader *reader) {
  assert(wsky_StringReader_hasMore(reader));

  char c = reader->string[reader->position.index++];
  if (c == '\n') {
    reader->position.line++;
    reader->position.column = 0;
  } else {
    reader->position.column++;
  }
  return c;
}

bool wsky_StringReader_readString(StringReader *reader, const char *string) {
  Position begin = reader->position;
  while (*string) {
    if (!wsky_StringReader_hasMore(reader)) {
      reader->position = begin;
      return false;
    }

    char c = wsky_StringReader_next(reader);
    if (c != *string) {
      reader->position = begin;
      return false;
    }
    string++;
  }
  return true;
}

int wsky_StringReader_skip(StringReader *reader, const char *charsToSkip) {
  int count = 0;
  while (wsky_StringReader_hasMore(reader)) {
    Position previous = reader->position;
    char c = wsky_StringReader_next(reader);
    if (strchr(charsToSkip, c) == NULL) {
      reader->position = previous;
      break;
    }
    count++;
  }
  return count;
}

int wsky_StringReader_skipWhitespaces(StringReader *reader) {
  return wsky_StringReader_skip(reader, " \t\n\r");
}

Token wsky_StringReader_createToken(StringReader *reader,
                                    Position begin,
                                    wsky_TokenType type) {
  const char *stringBegin = reader->string + begin.index;
  int length = reader->position.index - begin.index;
  char *string = wsky_strndup(stringBegin, (unsigned)length);
  Token t = wsky_Token_create(begin, reader->position, string, type);
  wsky_free(string);
  return t;
}
