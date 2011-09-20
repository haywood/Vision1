/**
 * p2.c
 * Program that implements the sequential labeling algorithm.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hw2.h"

#define NNEIGHB 3
enum {
	WEST,
	NORTHWEST,
	NORTH
};

void binaryToLabeled(char *, char *);
int isBinary(Image *);
void labelPixel(LabelMap *, int, int);
void resolvePixel(LabelMap *, int, int);
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
	int *labels;
	int i, j;
	LabelMap lm=makeLabelMap(im);

	rows=getNRows(im);
	cols=getNCols(im);

	labels=(int *)malloc(rows*cols*sizeof(int));
	if (labels == NULL) {
		fprintf(stderr, "memory error\n");
		exit(2);
	}

	for (i=0; i < rows; ++i)
		for (j=0; j < cols; ++j)
			labelPixel(&lm, i, j);

	/* reduce the equivalence classes of the labels 
	 * so that their indices are consecutive */
	reduceLabels(&lm); 

	for (i=0; i < rows; ++i)
		for (j=0; j < cols; ++j)
			resolvePixel(&lm, i, j);
	
	setColors(im, getNClasses(&lm));
	freeLabelMap(&lm);
}

int getNeighbors(LabelMap *lm, int i, int j, int neighbors[NNEIGHB])
{
	int has=0;
	memset(neighbors, 0, sizeof neighbors);
	if (j) has+=neighbors[WEST]=getLabel(lm, i, j-1);
	if (i && j) has+=neighbors[NORTHWEST]=getLabel(lm, i-1, j-1);
	if (i) has+=neighbors[NORTH]=getLabel(lm, i-1, j);
	return !has;
}

int evalNeighbor(int k, LabelMap *lm, int neighbors[NNEIGHB])
{
	int l, c=neighbors[k];
	for (l=k+1; l < NNEIGHB; ++l)
		if (neighbors[l]) {
			setEquivalent(lm, neighbors[k], neighbors[l]);
			if (neighbors[l] > c) c=neighbors[l];
		}
	return c;
}

void labelPixel(LabelMap *lm, int i, int j) 
{
	if (getPixel(lm->im, i, j) > 0) {
		int neighbors[NNEIGHB], newObj, k, c;

		memset(neighbors, 0, sizeof neighbors);
		newObj=getNeighbors(lm, i, j, neighbors);

		if (!newObj) {
			for (k=0; k < NNEIGHB; ++k)
				if (neighbors[k]) {
					c=evalNeighbor(k, lm, neighbors);
					break; /* break since finding one means checking the rest */
				}
		} else {
			addLabel(lm);
			c=getNLabels(lm);
		}
		setLabel(lm, i, j, c);
		assert(getLabel(lm, i, j)==c);
		assert(lm->nClasses > 0);
	}
}

void resolvePixel(LabelMap *lm, int i, int j)
{
	int px=getPixel(lm->im, i, j);
	if (px > 0)
		resolveLabel(lm, i, j);
}

int isBinary(Image *im) { return getColors(im) == BINARY; }
