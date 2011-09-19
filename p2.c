/**
 * p2.c
 * Program that implements the sequential labeling algorithm.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vision_utilities.h"
#include "hw2.h"

#define NNEIGHB 3
enum {
	WEST,
	NORTHWEST,
	NORTH
};

void binaryToLabeled(char *, char *);
int isBinary(Image *);
void labelPixel(Image *, int, int, labelMap *);
void resolvePixel(Image *, int, int, labelMap *);
void sequentialLabeling(Image *);

int main(int argc, char *argv[])
{
	char *ifname, *ofname;

	if (argc < 3) {
		printf("usage: %s <input image> <output image>\n", argv[0]);
		return 0;
	}

	ifname=argv[1];
	ofname=argv[2];
	binaryToLabeled(ifname, ofname);
	return 0;
}

void binaryToLabeled(char *ifname, char *ofname)
{
	Image im;

	memset(&im, 0, sizeof im);

	/* Read the binary image */

	if (readImage(&im, ifname)==-1) {
		fprintf(stderr, "unable to read binary image");
		exit(1);
	}

	else if (!isBinary(&im)) {
		fprintf(stderr, "input file is not a binary image");
		exit(1);
	}

	sequentialLabeling(&im);

	if (writeImage(&im, ofname) == -1) {
		fprintf(stderr, "unable to write binary image");
		exit(1);
	}

	free(im.data);
}

void sequentialLabeling(Image *im)
{
	int rows, cols;
	int i, j;
	labelMap lm;

	memset(&lm, 0, sizeof lm); /* initialize memory */

	rows=getNRows(im);
	cols=getNCols(im);

	for (i=0; i < rows; ++i)
		for (j=0; j < cols; ++j)
			labelPixel(im, i, j, &lm);

	/* reduce the equivalence classes of the labels 
	 * so that their indices are consecutive */
	reduceLabels(&lm); 

	for (i=0; i < rows; ++i)
		for (j=0; j < cols; ++j)
			resolvePixel(im, i, j, &lm);
	
	printf("%d\n", getNClasses(&lm));
	setColors(im, getNClasses(&lm));
	freeLabelMap(&lm);
}

int getNeighbors(Image *im, int i, int j, int neighbors[3])
{
	int has=0;
	if (j) has+=neighbors[WEST]=getPixel(im, i, j-1); /* west */
	if (i && j) has+=neighbors[NORTHWEST]=getPixel(im, i-1, j-1); /* northwest */
	if (i) has+=neighbors[NORTH]=getPixel(im, i-1, j); /* north */
	return has;
}

int evalNeighbor(int k, labelMap *lm, int neighbors[3])
{
	int l, c=neighbors[k];
	for (l=k+1; l < NNEIGHB; ++l)
		if (neighbors[l]) {
			setEquivalent(lm, neighbors[k], neighbors[l]);
			if (neighbors[l] > neighbors[k]) c=neighbors[l];
		}
	return c;
}

void labelPixel(Image *im, int i, int j, labelMap *lm) 
{
	if (getPixel(im, i, j) > 0) {
		int neighbors[3], newObj, k, c;

		memset(neighbors, 0, sizeof neighbors);
		newObj=getNeighbors(im, i, j, neighbors);

		if (newObj) {
			for (k=0; k < NNEIGHB; ++k)
				if (neighbors[k]) {
					c=evalNeighbor(k, lm, neighbors);
					break; /* break since finding one means checking the rest */
				}
		} else {
			addLabel(lm);
			c=getNLabels(lm);
		}
		setPixel(im, i, j, c);
		assert(lm->classes > 0);
	}
}

void resolvePixel(Image *im, int i, int j, labelMap *lm)
{
	int px=getPixel(im, i, j);
	if (px > 0)
		setPixel(im, i, j, getClass(lm, px));
}

int isBinary(Image *im) { return getColors(im) == BINARY; }
