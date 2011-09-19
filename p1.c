/**
 * p1.c
 * Program for converting a gray-level image to a binary one.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vision_utilities.h"
#include "hw2.h"

int grayscale_to_binary(const char *, int, const char *);
int b(Image *, int, int, int);

int main(int argc, char *argv[]){
	char *ifname, *ofname;
	int threshold;

	if (argc < 4){
		printf("usage: %s <input image> <threshold> <output image>\n", argv[0]);
		return 0;
	}

	ifname=argv[1];
	ofname=argv[3];
	if (sscanf(argv[2], "%d", &threshold) != 1){
		fprintf(stderr, "%s: threshold must be an integer\n", argv[0]); 
		return 1;
	}

	return grayscale_to_binary(ifname, threshold, ofname);
}

int grayscale_to_binary(const char * ifname, const int threshold, const char * ofname){
	int rows, cols;
	int i, j, ret;
	Image im;

	memset(&im, 0, sizeof im);
	ret=0;

	/* Read grayscale image */

	if (readImage(&im, ifname)==-1)
	{
		fprintf(stderr, "unable to read grayscale image");
		if (im.data != NULL) free(im.data);
		exit(1);
	}

	rows=getNRows(&im);
	cols=getNCols(&im);

	/* Convert to binary image according to threshold. */

	for (i=0; i < rows; ++i)
		for (j=0; j < cols; ++j)
			setPixel(&im, i, j, b(&im, i, j, threshold));

	setColors(&im, BINARY); /* binary color setting */

	/* Write the binary image to ofname */

	if (writeImage(&im, ofname) == -1)
	{
		fprintf(stderr, "unable to write binary image");
		ret=1;
	}


	/* Free memory */

	free(im.data);

	return ret;
}

int b(Image * im, int i, int j, int threshold){
	return getPixel(im, i, j) >= threshold ? 1 : 0;
}
