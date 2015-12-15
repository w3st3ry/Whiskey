
compiler = 'clang'

ccflags = ' '

if compiler == 'clang':
    ccflags += '-Weverything -Wno-padded -Wno-switch-enum '

ccflags += '-Wall -Wextra '
ccflags += '-g '
ccflags += '-Iinclude/ '

libs = 'm'

env = Environment(
    CC = compiler,
    CCFLAGS = ccflags,
    LIBS = libs,
)

sources = Split('''
ast.c
class.c
dict.c
eval.c
exception.c
gc.c
lexer.c
method.c
object.c
parser.c
position.c
program_file.c
return_value.c
scope.c
str.c
string_reader.c
syntax_error.c
token.c
value.c
''')

env.wsky_objects = env.Object(sources)

SConscript('test/SConscript', 'env')
env.Program('whiskey', env.wsky_objects + ['main.c'])
