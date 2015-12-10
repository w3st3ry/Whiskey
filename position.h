#ifndef L_POSITION_H_
# define L_POSITION_H_

# include <stdio.h>
# include "program_file.h"



struct degat_Position_s;
typedef struct degat_Position_s degat_Position;

struct degat_Position_s
{
  degat_ProgramFile *file;
  int index;
  int line;
  int column;
};

bool degat_Position_equals(const degat_Position *a, const degat_Position *b);

void degat_Position_print(const degat_Position *this, FILE *file);
char *degat_Position_toString(const degat_Position *this);



#endif /* !L_POSITION_H_ */
