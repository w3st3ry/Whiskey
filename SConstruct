import os

print("Type 'scons -h' for more information")

Help('''
Options:
        CC      Sets the default compiler
        VERBOSE More verbose output if set to 1
''')

subdirs = 'objects repl modules'.split()
include_dirs = 'include'.split()

compiler = ARGUMENTS.get('CC', 'cc')

env = Environment(
    CC=compiler,
)

if ARGUMENTS.get('VERBOSE') != '1':
    env['CCCOMSTR'] = "Compiling $TARGET"
    env['LINKCOMSTR'] = "Linking $TARGET"

conf = Configure(env)
if conf.CheckLib('readline'):
    conf.env.Append(CCFLAGS = '-DHAVE_READLINE')

if conf.CheckFunc('strdup'):
    conf.env.Append(CCFLAGS = '-DHAVE_STRDUP')

if conf.CheckFunc('strndup'):
    conf.env.Append(CCFLAGS = '-DHAVE_STRNDUP')

env = conf.Finish()

env.Append(LIBS = 'm')

def get_compiler_flags(compiler):
    ccflags = ''
    if compiler.startswith('clang'):
       ccflags += '-Weverything -Wno-padded -Wno-switch-enum '

    ccflags += '-std=c99 -Wall -Wextra -Wpedantic '

    for include_dir in include_dirs:
        ccflags += '-I' + include_dir + ' '

    ccflags += '-g '

    return ccflags

env.Append(CCFLAGS = get_compiler_flags(compiler))

env['ENV']['TERM'] = os.environ['TERM']
env['ENV']['GCC_COLORS'] = ('error=01;31:warning=01;35:note=01;36:'
                            'caret=01;32:locus=01:quote=01')

env.subdirs = subdirs

objects = []

o = SConscript('src/SConscript', 'env')
objects.append(o)

env.wsky_objects = objects

test_binary = SConscript('test/SConscript', 'env')

whiskey = env.Program('whiskey', env.wsky_objects + ['src/main.c'])
Default(whiskey)

env.Command('test', test_binary,
            './$SOURCE')

env.Command('vgtest', test_binary,
            'valgrind --leak-check=full --track-origins=yes --suppressions=valgrind.supp ./$SOURCE')

env.Command('vg', whiskey,
            'valgrind --leak-check=full --track-origins=yes --suppressions=valgrind.supp ./$SOURCE')
