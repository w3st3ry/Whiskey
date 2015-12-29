#ifndef STRING_READER_H_
# define STRING_READER_H_

#include "token.h"
#include "objects/program_file.h"


/**
 * @defgroup StringReader StringReader
 * @{
 */

/**
 * A StringReader
 */
typedef struct wsky_StringReader_s {

  /** The current position of the reader */
  wsky_Position position;

  /** The file or `NULL` */
  wsky_ProgramFile *file;

  /** The string to read */
  const char *string;
} wsky_StringReader;


/**
 * Creates a reader.
 * @param file The file or NULL
 * @param string The string
 */
wsky_StringReader wsky_StringReader_create(wsky_ProgramFile *file,
                                           const char *string);

/**
 * Frees the given reader.
 */
void wsky_StringReader_free(wsky_StringReader *reader);

/**
 * Reads a character.
 * Increments the position of the reader.
 */
char wsky_StringReader_next(wsky_StringReader *reader);

/**
 * Reads the given string.
 * Returns true if the string has been read, or false otherwise.
 */
bool wsky_StringReader_readString(wsky_StringReader *reader,
                                  const char *string);

/**
 * Skips characters.
 * Returns the number of skipped characters.
 * @param reader The reader
 * @param charsToSkip A null-terminated string of the characters to skip.
 */
int wsky_StringReader_skip(wsky_StringReader *reader,
                           const char *charsToSkip);

/**
 * Skips line breaks, tabulations and spaces.
 * Returns the number of skipped characters
 */
int wsky_StringReader_skipWhitespaces(wsky_StringReader *reader);

/**
 * Returns false if the reader is at the end of the string.
 */
bool wsky_StringReader_hasMore(const wsky_StringReader *reader);

/**
 * Creates a wsky_Token.
 */
wsky_Token wsky_StringReader_createToken(wsky_StringReader *reader,
                                         wsky_Position begin,
                                         wsky_TokenType type);

/**
 * @}
 */

#endif /* !STRING_READER_H_ */
