#format is target-name: target dependencies
#{-tab-}actions

# Executable "task1c" depends on the file task1c.o.
virus_detector: virus_detector.o
	gcc -m32 -g -Wall -o virus_detector virus_detector.o

# Depends on "task1c" source file
virus_detector.o: virus_detector.c
	gcc -g -Wall -m32 -ansi -c -o virus_detector.o virus_detector.c
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o virus_detector

