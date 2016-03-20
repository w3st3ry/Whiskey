import os

subdirs = 'objects repl modules'.split()
include_dirs = 'include'.split()

def get_compiler_flags(compiler):
    ccflags = ''
    if compiler.startswith('clang'):
       ccflags += '-Weverything -Wno-padded -Wno-switch-enum '

    ccflags += '-std=c99 -Wall -Wextra -Wpedantic '
    ccflags += '-g -fstack-protector-all -fstack-protector-strong '

    for include_dir in include_dirs:
        ccflags += '-I' + include_dir + ' '
    return ccflags


libs = 'm'.split()

compiler = ARGUMENTS.get('CC', 'cc')

env = Environment(
    CC=compiler,
    LIBS=libs,
)

env.Append(CCFLAGS = get_compiler_flags(compiler))

env['ENV']['TERM'] = os.environ['TERM']
env['ENV']['GCC_COLORS'] = ('error=01;31:warning=01;35:note=01;36:'
                            'caret=01;32:locus=01:quote=01')

env.subdirs = subdirs

objects = []

o = SConscript('src/SConscript', 'env')
objects.append(o)

env.wsky_objects = objects

SConscript('test/SConscript', 'env')
env.Program('whiskey', env.wsky_objects + ['src/main.c'])
