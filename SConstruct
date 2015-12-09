
ccflags = '-Weverything -Wno-padded '
ccflags += '-g'

libs = 'm'

env = Environment(
    CC = 'clang',
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
value.c
''')

env.degat_objects = env.Object(sources)

SConscript('test/SConscript', 'env')
Program('degat', env.degat_objects + ['main.c'])
