
J	= 1

all:
	scons CC=$(CC) -j $(J)

doc:
	doxygen Doxyfile

clean:
	scons -c
	rm -Rf doxyoutput

re: clean all

.PHONY: all clean re
