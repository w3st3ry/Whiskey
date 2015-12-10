#include "string_reader.h"

#include <stdlib.h>
#include <string.h>
#include "gc.h"


degat_StringReader degat_StringReader_create(degat_ProgramFile *file) {
  degat_Object_INCREF(file);
  degat_Position pos = {
    .index = 0,
    .line = 1,
    .column = 0,
    .file = file,
  };
  degat_StringReader reader = {
    .file = file,
    .string = file->content,
    .position = pos,
  };
  return reader;
}

degat_StringReader *degat_StringReader_new(degat_ProgramFile *file) {
  degat_StringReader *reader = malloc(sizeof(degat_StringReader));
  if (!reader)
    return NULL;
  *reader = degat_StringReader_create(file);
  return reader;
}

void degat_StringReader_delete(degat_StringReader *reader) {
  degat_Object_DECREF(reader->file);
}



bool degat_StringReader_hasMore(const degat_StringReader *reader) {
  return (reader->position.index < (int)strlen(reader->string));
}

char degat_StringReader_next(degat_StringReader *reader) {
  if (!degat_StringReader_hasMore(reader))
    abort();
  return '.';
}
