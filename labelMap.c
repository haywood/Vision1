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
	labelMap lm={0, 0, NULL, NULL, NULL, NULL};
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

int getNLabels(labelMap *lm) { return lm->nLabels; }

void resizeClass(labelMap *lm, int c, int newSize)
{
	lm->classes[c]=(int *)realloc(lm->classes[c], (newSize+1)*sizeof(int));
	if (lm->classes[c] == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	lm->classes[c][0]=newSize;
}

void addLabel(labelMap *lm)
{
	int n, m;

	n=lm->nLabels;
	m=++lm->nLabels;

	lm->classes=(int **)realloc(lm->classes, m*sizeof(int *));
	lm->map=(int *)realloc(lm->map, m*sizeof(int));
	if (lm->map == NULL || lm->classes == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	lm->classes[n]=(int *)malloc(2*sizeof(int));
	lm->classes[n][0]=1;
	lm->classes[n][1]=n;
	lm->map[n]=m;
	lm->nClasses++;
}

void mergeClasses(labelMap *lm, int toClass, int frClass)
{
	int *top, *frp, toSize, frSize, i;

	top=lm->classes[toClass];
	frp=lm->classes[frClass];
	toSize=top[0];
	frSize=frp[0];

	resizeClass(lm, toClass, toSize+frSize);

	top=lm->classes[toClass];
	frp=lm->classes[frClass];

	memcpy((void *) (top+toSize+1), (void *) (frp+1), frSize*sizeof(int));

	for (i=1; i <= frSize; ++i)
		lm->map[frp[i]]=toClass+1;

	resizeClass(lm, frClass, 0);
}

void setEquivalent(labelMap *lm, int i, int j)
{ 
	i=ITRANS(i);
	j=ITRANS(j);

	if (lm->map[i] == lm->map[j]) return; /* already equivalent */
	else if (lm->map[j] < lm->map[i]) {
		mergeClasses(lm, lm->map[j]-1, lm->map[i]-1);
	} else mergeClasses(lm, lm->map[i]-1, lm->map[j]-1);
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
	int i, count=0;
	
	for (i=0; i < lm->nLabels; ++i)
		if (lm->classes[i][0]) count++;

	if (count != lm->nClasses) {
		fprintf(stderr, "class verification failed. expected: %d, got: %d\n", lm->nClasses, count);
		exit(2);
	}
}

void reduceLabels(labelMap *lm)
{
	int i, c=0;

	verifyMap(lm);
	i=lm->nClasses;

	while (c < lm->nClasses) {
		while (lm->classes[c][0]) c++;
		if (lm->classes[i][0]) { 
			mergeClasses(lm, c, i);
		}
		free(lm->classes[i]);
		i++;
	}
	lm->classes=(int **)realloc(lm->classes, lm->nClasses*sizeof(int*));
	lm->nLabels=lm->nClasses;
}

void printClasses(labelMap *lm)
{
	int i, *printed=(int *)malloc(lm->nLabels*sizeof(int));
	memset(printed, 0, lm->nLabels*sizeof(int));
	for (i=0; i < lm->nLabels; ++i) {
		if (!printed[lm->map[i]]) {
			printf("%d\n", lm->map[i]);
			printed[lm->map[i]]=1;
		}
	}
	free(printed);
}

void freeLabelMap(labelMap *lm)
{
	int i;
	free(lm->map);
	free(lm->labels);
	for (i=0; i < lm->nLabels; ++i)
		free(lm->classes[i]);
	free(lm->classes);
}
