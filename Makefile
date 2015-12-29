
all:
	scons

doc:
	doxygen Doxyfile

clean:
	scons -c
	rm -Rf doxyoutput

re: clean all

.PHONY: all clean re
