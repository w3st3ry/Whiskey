#include "position.h"

#include <string.h>
#include <stdlib.h>

bool wsky_Position_equals(const wsky_Position *a, const wsky_Position *b) {
  if (a->file != b->file)
    abort();
  if (a->index == b->index)
    {
      if ((a->line != b->line) || (a->column != b->column))
	{
	  abort();
	}
      return true;
    }
  return false;
}
