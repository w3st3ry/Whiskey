#include "string_reader.h"

#include <stdlib.h>
#include <string.h>
#include "gc.h"


wsky_StringReader wsky_StringReader_create(wsky_ProgramFile *file,
					   const char *string) {

  wsky_XINCREF(file);
  if (!file && !string) {
    abort();
  }
  wsky_Position pos = {
    .index = 0,
    .line = 1,
    .column = 0,
    .file = file,
  };
  wsky_StringReader reader = {
    .file = file,
    .string = string,
    .position = pos,
  };
  return reader;
}

wsky_StringReader *wsky_StringReader_newFile(wsky_ProgramFile *file) {
  return wsky_StringReader_new(file, NULL);
}

wsky_StringReader *wsky_StringReader_new(wsky_ProgramFile *file,
					 const char *string) {
  wsky_StringReader *reader = malloc(sizeof(wsky_StringReader));
  if (!reader)
    return NULL;
  *reader = wsky_StringReader_create(file, string);
  return reader;
}

wsky_StringReader *wsky_StringReader_newStr(const char *string) {
  return wsky_StringReader_new(NULL, string);
}

void wsky_StringReader_delete(wsky_StringReader *reader) {
  wsky_XDECREF(reader->file);
  free(reader);
}



bool wsky_StringReader_hasMore(const wsky_StringReader *reader) {
  return (reader->position.index < (int)strlen(reader->string));
}

char wsky_StringReader_next(wsky_StringReader *reader) {
  if (!wsky_StringReader_hasMore(reader))
    abort();
  char c = reader->string[reader->position.index++];
  if (c == '\n') {
    reader->position.line++;
    reader->position.column = 0;
  } else {
    reader->position.column++;
  }
  return c;
}

bool wsky_StringReader_readString(wsky_StringReader *reader,
				  const char *string) {
  wsky_Position begin = reader->position;
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

int wsky_StringReader_skip(wsky_StringReader *reader,
			   const char *charsToSkip) {

  int count = 0;
  while (wsky_StringReader_hasMore(reader)) {
    wsky_Position previous = reader->position;
    char c = wsky_StringReader_next(reader);
    if (strchr(charsToSkip, c) == NULL) {
      reader->position = previous;
      break;
    }
    count++;
  }
  return count;
}

int wsky_StringReader_skipWhitespaces(wsky_StringReader *reader) {
  return wsky_StringReader_skip(reader, " \t\n\r");
}

wsky_Token wsky_StringReader_createToken(wsky_StringReader *reader,
					 wsky_Position begin,
					 wsky_TokenType type) {
  const char *stringBegin = reader->string + begin.index;
  int length = reader->position.index - begin.index;
  char *string = strndup(stringBegin, (unsigned)length);
  wsky_Token t = wsky_Token_create(begin, reader->position, string, type);
  free(string);
  return  t;
}
