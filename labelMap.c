/**
 * labelMap.c
 * implements functions for the labelMap struct
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "hw2.h"

#define ITRANS(x) x-1
#define OTRANS(x) x+1

labelMap makeLabelMap(Image *im)
{
	labelMap lm;
	memset(&lm, 0, sizeof lm);
	lm.labels=(int *)malloc(getNRows(im)*getNCols(im)*sizeof(int));
	memset(lm.labels, 0, sizeof(getNRows(im)*getNCols(im)*sizeof(int)));
	lm.im=im;
	return lm;
}

int getLabel(labelMap *lm, int i, int j)
{
	return lm->labels[i*getNCols(lm->im)+j];
}

void setLabel(labelMap *lm, int i, int j, int l)
{
	lm->labels[i*getNCols(lm->im)+j]=l;
}

void resolveLabel(labelMap *lm, int i, int j)
{
	int l=getLabel(lm, i, j);
	if (l > 0) {
		int c=getClass(lm, l);
		if (c <= PIXEL_MAX) {
			setLabel(lm, i, j, c);
			setPixel(lm->im, i, j, c);
		} else {
			fprintf(stderr, "there are too many objects in the image\n");
			exit(2);
		}
	}
}

Image *getImage(labelMap *lm)
{
	return lm->im;
}

int getNLabels(labelMap *lm) { return lm->number; }

void addLabel(labelMap *lm)
{
	int n, m;

	n=lm->number;
	m=++lm->number;

	lm->map=realloc(lm->map, m*sizeof(int));
	if (lm->map == NULL) {
		fprintf(stderr, "error adding new label to map\n");
		exit(1);
	}
	lm->map[n]=m;
	lm->nClasses++;
}

void setEquivalent(labelMap *lm, int i, int j)
{ 
	int k, class;

	i=ITRANS(i);
	j=ITRANS(j);

	if (lm->map[i] == lm->map[j]) return; /* already equivalent */
	else if (lm->map[j] < lm->map[i]) {
		class=j;
		j=i;
		i=class;
	}

	for (k=0; k < lm->number; ++k)
		if (lm->map[k]==lm->map[j])
			lm->map[k]=lm->map[i];
	lm->nClasses--;
}

int isEquivalent(labelMap *lm, int i, int j)
{ 
	return lm->map[ITRANS(i)] == lm->map[ITRANS(j)];
}

int getClass(labelMap *lm, int i)
{
	return lm->map[ITRANS(i)];
}

int getNClasses(labelMap *lm)
{
	return lm->nClasses;
}

void verifyMap(labelMap *lm)
{
	int i, *classes, count=0;
	
	classes=(int *)malloc(lm->number*sizeof(int));
	memset(classes, 0, lm->number*sizeof(int));

	for (i=0; i < lm->number; ++i)
		if (!classes[lm->map[i]-1])
			count+=classes[lm->map[i]-1]=1;
	if (count != lm->nClasses) {
		fprintf(stderr, "class verification failed. expected: %d, got: %d\n", lm->nClasses, count);
		exit(2);
	}
}

void reduceLabels(labelMap *lm)
{
	int *taken, i, k, class[2], c=0;

	verifyMap(lm);

	taken=(int *)malloc(lm->nClasses*sizeof(int));
	memset(taken, 0, lm->nClasses*sizeof(int));

	for (i=0; i < lm->number; ++i)
		if (lm->map[i] <= lm->nClasses)
			taken[lm->map[i]-1]=1;

	for (i=0; i < lm->number; ++i) {
		/* the label's equivalence class has an illegal value 
		 * if it is greater than the number of existing classes */
		if (lm->map[i] > lm->nClasses) { 
			/* the first break in the sequence of assigned classes is found
			 * and the illegal value is mapped to it throughout 
			 * the list of labels */
			for (k=0; k < lm->nClasses; ++k)
				if (!taken[k]) break;
			taken[k]=1;

			class[0]=lm->map[i]; /* from */
			class[1]=k+1; /* to */

			/* give the selected class its new legal value
			 * this loop only goes through labels after 
			 * and including the one selected since earlier labels
			 * have already been accounted for
			 */
			for (k=i; k < lm->number; ++k)
				if (lm->map[k] == class[0])
					lm->map[k]=class[1];

			/* check that all classes are not legal */
			c=0;
			for (k=0; k < lm->nClasses; ++k)
				if (!taken[k]) {
					c=1;
					break;
				}
			if (!c) break;
		}
	}

	free(taken);
}

void printClasses(labelMap *lm)
{
	int i, *printed=(int *)malloc(lm->number*sizeof(int));
	memset(printed, 0, lm->number*sizeof(int));
	for (i=0; i < lm->number; ++i) {
		if (!printed[lm->map[i]]) {
			printf("%d\n", lm->map[i]);
			printed[lm->map[i]]=1;
		}
	}
}

void freeLabelMap(labelMap *lm)
{
	free(lm->map);
	free(lm->labels);
}
