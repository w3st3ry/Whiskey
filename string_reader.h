#ifndef STRING_READER_H_
# define STRING_READER_H_

#include "token.h"


struct wsky_StringReader_s;
typedef struct wsky_StringReader_s wsky_StringReader;

/* This structure is not a garbage-collected object. */
struct wsky_StringReader_s {
  wsky_Position position;
  wsky_ProgramFile *file;
  const char *string;
};

wsky_StringReader wsky_StringReader_create(wsky_ProgramFile *file,
					   const char *string);
wsky_StringReader *wsky_StringReader_newStr(const char *string);
wsky_StringReader *wsky_StringReader_newFile(wsky_ProgramFile *file);
wsky_StringReader *wsky_StringReader_new(wsky_ProgramFile *file,
					 const char *string);

void wsky_StringReader_delete(wsky_StringReader *reader);


char wsky_StringReader_next(wsky_StringReader *reader);

/** Returns true if the string has been read, or false otherwise */
bool wsky_StringReader_readString(wsky_StringReader *reader,
				  const char *string);

/* Returns the number of skipped characters */
int wsky_StringReader_skip(wsky_StringReader *reader,
			   const char *charsToSkip);

/* Returns the number of skipped characters */
int wsky_StringReader_skipWhitespaces(wsky_StringReader *reader);

bool wsky_StringReader_hasMore(const wsky_StringReader *reader);

wsky_Token wsky_StringReader_createToken(wsky_StringReader *reader,
					 wsky_Position begin,
					 wsky_TokenType type);

#endif /* !STRING_READER_H_ */
