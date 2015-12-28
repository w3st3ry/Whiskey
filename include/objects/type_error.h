#ifndef TYPE_ERROR_H_
# define TYPE_ERROR_H_


# include "exception.h"


extern wsky_Class wsky_TypeError_CLASS;

typedef struct wsky_TypeError_s wsky_TypeError;

struct wsky_TypeError_s {
  wsky_Exception_HEAD
};



wsky_TypeError *wsky_TypeError_new(const char *message);


#endif /* !TYPE_ERROR_H_ */
