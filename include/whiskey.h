#ifndef WHISKEY_H_
# define WHISKEY_H_

# include <stdint.h>

/**
 * @defgroup whiskey whiskey
 * @{
 */

/** A Whiskey integer */
typedef int64_t wsky_int;

/** A Whiskey float */
typedef double wsky_float;


/** Starts Whiskey */
void wsky_start(void);

/** Stops Whiskey */
void wsky_stop(void);

/**
 * @}
 */

#endif /* !WHISKEY_H_ */
