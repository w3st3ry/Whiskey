#ifndef STRING_READER_H_
# define STRING_READER_H_

#include "position.h"


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
void degat_StringReader_delete(degat_StringReader *reader);
bool degat_StringReader_hasMore(const degat_StringReader *reader);
char degat_StringReader_next(degat_StringReader *reader);



#endif /* !STRING_READER_H_ */
