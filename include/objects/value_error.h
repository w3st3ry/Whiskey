#ifndef VALUE_ERROR_H_
# define VALUE_ERROR_H_


# include "exception.h"

extern wsky_Class wsky_ValueError_CLASS;

typedef struct wsky_ValueError_s wsky_ValueError;

struct wsky_ValueError_s {
  wsky_Exception_HEAD
};


wsky_ValueError *wsky_ValueError_new(const char *message);


#endif /* !VALUE_ERROR_H_ */