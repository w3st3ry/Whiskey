
all:
	scons CC=$(CC)

doc:
	doxygen Doxyfile

clean:
	scons -c CC=$(CC)
	rm -Rf doxyoutput

re: clean all

.PHONY: all clean re
