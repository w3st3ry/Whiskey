#ifndef WHISKEY_H_
# define WHISKEY_H_

/*   __        ___     _     _
 *   \ \      / / |__ (_)___| | _____ _   _
 *    \ \ /\ / /| '_ \| / __| |/ / _ \ | | |
 *     \ V  V / | | | | \__ \   <  __/ |_| |
 *      \_/\_/  |_| |_|_|___/_|\_\___|\__, |
 *                                    |___/
 */

# include "ast.h"
# include "class_def.h"
# include "dict.h"
# include "eval.h"
# include "gc.h"
# include "keyword.h"
# include "lexer.h"
# include "memory.h"
# include "method_def.h"
# include "operator.h"
# include "parser.h"
# include "path.h"
# include "position.h"
# include "string_reader.h"
# include "string_utils.h"
# include "syntax_error.h"
# include "token.h"

# include "objects/attribute_error.h"
# include "objects/boolean.h"
# include "objects/class.h"
# include "objects/exception.h"
# include "objects/float.h"
# include "objects/function.h"
# include "objects/import_error.h"
# include "objects/instance_method.h"
# include "objects/integer.h"
# include "objects/method.h"
# include "objects/module.h"
# include "objects/name_error.h"
# include "objects/not_implemented_error.h"
# include "objects/null.h"
# include "objects/exception.h"
# include "objects/object.h"
# include "objects/parameter_error.h"
# include "objects/program_file.h"
# include "objects/scope.h"
# include "objects/str.h"
# include "objects/structure.h"
# include "objects/syntax_error_ex.h"
# include "objects/type_error.h"
# include "objects/value_error.h"
# include "objects/zero_division_error.h"

# include "modules/math.h"

# include "repl/repl.h"

/**
 * @defgroup whiskey whiskey
 * @{
 */

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
