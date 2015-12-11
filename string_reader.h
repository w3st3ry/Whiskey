#ifndef STRING_READER_H_
# define STRING_READER_H_

#include "token.h"


struct wsky_StringReader_s;
typedef struct wsky_StringReader_s wsky_StringReader;

/* This structure is not a garbage-collected object. */
struct wsky_StringReader_s
{
  wsky_Position position;
  wsky_ProgramFile *file;
  const char *string;
};

wsky_StringReader wsky_StringReader_create(wsky_ProgramFile *file,
					     const char *string);
wsky_StringReader *wsky_StringReader_newStr(const char *string);
wsky_StringReader *wsky_StringReader_new(wsky_ProgramFile *file);

char wsky_StringReader_next(wsky_StringReader *reader);
void wsky_StringReader_delete(wsky_StringReader *reader);
bool wsky_StringReader_hasMore(const wsky_StringReader *reader);

wsky_Token wsky_StringReader_createToken(wsky_StringReader *reader,
					   wsky_Position begin);

#endif /* !STRING_READER_H_ */
