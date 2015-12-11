
use_clang = True

if use_clang:
    ccflags = '-Weverything -Wno-padded '
else:
    ccflags = ' '

ccflags += '-Wall -Wextra '
ccflags += '-g '

libs = 'm'

env = Environment(
    CC = 'clang' if use_clang else 'gcc',
    CCFLAGS = ccflags,
    LIBS = libs,
)

sources = Split('''
class.c
exception.c
gc.c
method.c
object.c
position.c
program_file.c
return_value.c
str.c
string_reader.c
token.c
value.c
''')

env.wsky_objects = env.Object(sources)

SConscript('test/SConscript', 'env')
env.Program('wsky', env.wsky_objects + ['main.c'])
