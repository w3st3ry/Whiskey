#ifndef STRING_READER_H_
# define STRING_READER_H_

#include "token.h"


struct degat_StringReader_s;
typedef struct degat_StringReader_s degat_StringReader;

/* This structure is not a garbage-collected object. */
struct degat_StringReader_s
{
  degat_Position position;
  degat_ProgramFile *file;
  const char *string;
};

degat_StringReader degat_StringReader_create(degat_ProgramFile *file,
					     const char *string);
degat_StringReader *degat_StringReader_newStr(const char *string);
degat_StringReader *degat_StringReader_new(degat_ProgramFile *file);

char degat_StringReader_next(degat_StringReader *reader);
void degat_StringReader_delete(degat_StringReader *reader);
bool degat_StringReader_hasMore(const degat_StringReader *reader);

degat_Token degat_StringReader_createToken(degat_StringReader *reader,
					   degat_Position begin);

#endif /* !STRING_READER_H_ */
