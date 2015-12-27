
compiler = 'clang'

ccflags = ' '

if compiler == 'clang':
    ccflags += '-Weverything -Wno-padded -Wno-switch-enum '

subdirs = ['objects', 'repl']

include_dirs = ['include']

ccflags += '-Wall -Wextra '
ccflags += '-g '

for include_dir in include_dirs:
    ccflags += '-I' + include_dir + ' '

libs = 'm'.split()

env = Environment(
    CC = compiler,
    CCFLAGS = ccflags,
    LIBS = libs,
)

env.subdirs = subdirs

objects = []

o = SConscript('src/SConscript', 'env')
objects.append(o)

env.wsky_objects = objects

SConscript('test/SConscript', 'env')
env.Program('whiskey', env.wsky_objects + ['src/main.c'])
