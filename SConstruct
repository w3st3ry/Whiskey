
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
gc.c
keyword.c
lexer.c
method.c
object.c
parser.c
position.c
return_value.c
string_reader.c
syntax_error.c
token.c
value.c
''')

objects = []
for subdir in ['objects']:
    o = SConscript(subdir + '/SConscript', 'env')
    objects.append(o)

env.wsky_objects = objects + env.Object(sources)

SConscript('test/SConscript', 'env')
env.Program('whiskey', env.wsky_objects + ['main.c'])
