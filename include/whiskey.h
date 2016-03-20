#ifndef WHISKEY_H_
# define WHISKEY_H_

# include <stdint.h>
# include <stdbool.h>


/*   __        ___     _     _
 *   \ \      / / |__ (_)___| | _____ _   _
 *    \ \ /\ / /| '_ \| / __| |/ / _ \ | | |
 *     \ V  V / | | | | \__ \   <  __/ |_| |
 *      \_/\_/  |_| |_|_|___/_|\_\___|\__, |
 *                                    |___/
 */


/**
 * @defgroup whiskey whiskey
 * @{
 */

/** A Whiskey integer */
typedef int64_t wsky_int;

/** A Whiskey float */
typedef double wsky_float;

/** Returns true if Whiskey is started */
bool wsky_isStarted(void);

/** Starts Whiskey */
void wsky_start(void);

/** Stops Whiskey */
void wsky_stop(void);

/**
 * @}
 */

#endif /* !WHISKEY_H_ */
