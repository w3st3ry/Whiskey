
J	= 1

all:
	scons CC=$(CC) -j $(J)

vg:
	scons CC=$(CC) -j $(J) vg

test:
	scons CC=$(CC) -j $(J) test

vgtest:
	scons CC=$(CC) -j $(J) vgtest

doc:
	doxygen Doxyfile

clean:
	scons -c
	rm -Rf doxyoutput

re: clean all

.PHONY: all vg test vgtest doc clean re
