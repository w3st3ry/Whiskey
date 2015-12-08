
all:
	scons

clean:
	scons -c

re: clean all

.PHONY: all clean re
