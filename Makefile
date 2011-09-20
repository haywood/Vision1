# Makefile for computer vision hw2

# Your UNI
UNI = mhr2126

# VARIOUS VARIABLES (you will need to change some of these)
# The compiler. If you're writing code using c++, change gcc to g++
CC = gcc

# Compile time flags. If you want to compile with debugging turned off, remove the -g
CFLAGS = -g3 -O0 -Wall -Wextra -Werror -Wfatal-errors -pedantic

# Libraries to link against. You should only need to link against the math library (-lm).
LFLAGS = -lm

# The utilities object. Leave this as is.
UTILS = vision_utilities.c
UTILSH = vision_utilities.h

# Enter the names of your source files here. If you have header files as well,
# then do not include them here (they will be autoincluded).
P1SRC = p1.c
P2SRC = p2.c LabelMap.c
P3SRC = p3.c Object.c
P4SRC = p4.c

# Threshold for binarization. Change this.
THRESHOLD = 111 

# This contains a list of various images and other files generated from 
# running your programs. By default, it contains the filenames of all files
# generated from running the 'test' target. If you generate any other files,
# make sure this is correct. This list of files is deleted when 'make clean'
# or 'make submit' are run (to prevent submitting all these files).
OUTPUTS_TO_CLEAN = p11.pgm p12.pgm p13.pgm p21.pgm p22.pgm p23.pgm p31.pgm p32.pgm p33.pgm p421.pgm p431.pgm p413.pgm p423.pgm data1 data2 data3 p1.dSYM p2.dSYM p3.dSYM p4.dSYM

# THE BUILD RULES (you shouldn't need to edit any of these)
# 'make test' builds all 4 programs and then tests them.
# You should check to see that the final output images p4xx.pgm are correct
test: all
	./p1 two_objects.pgm $(THRESHOLD) p11.pgm
	./p1 many_objects_1.pgm $(THRESHOLD) p12.pgm
	./p1 many_objects_2.pgm $(THRESHOLD) p13.pgm
	./p2 p11.pgm p21.pgm
	./p2 p12.pgm p22.pgm
	./p2 p13.pgm p23.pgm
	./p3 p21.pgm data1 p31.pgm
	./p3 p22.pgm data2 p32.pgm
	./p3 p23.pgm data3 p33.pgm
	./p4 p22.pgm data1 p421.pgm
	./p4 p23.pgm data1 p431.pgm
	./p4 p21.pgm data3 p413.pgm
	./p4 p22.pgm data3 p423.pgm
	

# 'make all' builds all 4 programs (but doesn't run them)
all: p1 p2 p3 p4

# 'make p1' builds p1 only
p1: $(P1SRC) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $(P1SRC) $(UTILS) $(LFLAGS)

# 'make p2' builds p2 only
p2: $(P2SRC) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $(P2SRC) $(UTILS) $(LFLAGS)

# 'make p3' builds p3 only
p3: $(P3SRC) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $(P3SRC) $(UTILS) $(LFLAGS)

# 'make p4' builds p4 only
p4: $(P4SRC) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $(P4SRC) $(UTILS) $(LFLAGS)

# the rule for building the vision utilities
vision_utilities.o: vision_utilities.c vision_utilities.h
	$(CC) -c -Wall -pedantic -o $@ vision_utilities.c

# 'make submit' creates a tar of all files in the directory
# NOTE 1: It also runs 'make clean' before submission.
# NOTE 2: Make sure to post the generated .tar.gz file to the Homework 2 Dropbox 
#         on CourseWorks
 submit: clean
	tar -czvf $(UNI)-hw2.tar.gz $(P1SRC) $(P2SRC) $(P3SRC) $(P4SRC) $(UTILS) $(UTILSH) Makefile README

# 'make extract' extracts all files into a new folder named UNI
extract: $(UNI).tar.gz
	mkdir -p $(UNI)
	tar -xvf $(UNI)-hw2.tar.gz -C $(UNI)/

	
# 'make clean' removes all output and temporary files
clean:
	-rm -rf p1 p2 p3 p4 vision_utilities.o $(OUTPUTS_TO_CLEAN) core CORE
