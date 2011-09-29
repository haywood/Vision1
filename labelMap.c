/**
 * LabelMap.c
 * implements functions for the LabelMap struct
 */

#include <assert.h>

#include "hw2.h"

#define ITRANS(x) x-1
#define OTRANS(x) x+1

LabelMap makeLabelMap(Image *im)
{
	LabelMap lm={0, 0, NULL, NULL, NULL, NULL};
	lm.labels=(int *)malloc(getNRows(im)*getNCols(im)*sizeof(int));
	memset(lm.labels, 0, getNRows(im)*getNCols(im)*sizeof(int));
	lm.im=im;
	return lm;
}

int getLabel(LabelMap *lm, int i, int j)
{
	return lm->labels[i*getNCols(lm->im)+j];
}

void setLabel(LabelMap *lm, int i, int j, int l)
{
	lm->labels[i*getNCols(lm->im)+j]=l;
}

void labelPixel(LabelMap *lm, int i, int j) 
{
	if (getPixel(lm->im, i, j) > 0) {
		int neighbors[NNEIGHB], newObj, k, c;
		newObj=getNeighbors(lm, i, 0, j, 0, neighbors);
		if (newObj) {
			addLabel(lm);
			c=getNLabels(lm);
		} else {
			for (k=0; k < NNEIGHB; ++k)
				if (neighbors[k]) {
					c=evalNeighbor(k, lm, neighbors);
					break; /* break since finding one means checking the rest */
				}
		}
		setLabel(lm, i, j, c);
	}
}

int getNeighbors(LabelMap *lm, int i, int imin, int j, int jmin, int neighbors[NNEIGHB])
{
	int has=0;
	memset(neighbors, 0, sizeof(int[NNEIGHB]));
	if (j > jmin) has+=neighbors[WEST]=getLabel(lm, i, j-1);
	if (i > imin && j > jmin) has+=neighbors[NORTHWEST]=getLabel(lm, i-1, j-1);
	if (i > imin) has+=neighbors[NORTH]=getLabel(lm, i-1, j);
	return !has;
}

int evalNeighbor(int k, LabelMap *lm, int neighbors[NNEIGHB])
{
	int l, c=neighbors[k];
	for (l=k+1; l < NNEIGHB; ++l)
		if (neighbors[l] > 0) {
			setEquivalent(lm, neighbors[k], neighbors[l]);
			if (neighbors[l] > c) c=neighbors[l]; /* always take the highest and therefore youngest label */
		}
	return c;
}

void resolveLabel(LabelMap *lm, int i, int j)
{
	int l=getLabel(lm, i, j);
	if (l > 0) {
		int c=getClass(lm, l);
		if (c <= PIXEL_MAX) {
			setLabel(lm, i, j, c);
			setPixel(lm->im, i, j, c);
		}	
	}
}

Image *getImage(LabelMap *lm)
{
	return lm->im;
}

int getNLabels(LabelMap *lm) { return lm->nLabels; }

void resizeClass(LabelMap *lm, int c, int newSize)
{
	lm->classes[c]=(int *)realloc(lm->classes[c], (newSize+1)*sizeof(int));
	if (lm->classes[c] == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	lm->classes[c][0]=newSize;
}

void addLabel(LabelMap *lm)
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

void mergeClasses(LabelMap *lm, int toClass, int frClass)
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

void setEquivalent(LabelMap *lm, int i, int j)
{ 
	i=ITRANS(i);
	j=ITRANS(j);

	if (lm->map[i] == lm->map[j]) return; /* already equivalent */
	else if (lm->map[j] < lm->map[i]) {
		mergeClasses(lm, lm->map[j]-1, lm->map[i]-1);
	} else mergeClasses(lm, lm->map[i]-1, lm->map[j]-1);
	lm->nClasses--;
}

int isEquivalent(LabelMap *lm, int i, int j)
{ 
	return lm->map[ITRANS(i)] == lm->map[ITRANS(j)];
}

int getClass(LabelMap *lm, int i)
{
	return lm->map[ITRANS(i)];
}

int getNClasses(LabelMap *lm)
{
	return lm->nClasses;
}

void reduceLabels(LabelMap *lm)
{
	int i, c=0;

	i=lm->nClasses;

	while (c < lm->nClasses) {
		while (c < lm->nClasses && lm->classes[c][0]) c++;
		if (c < lm->nClasses && lm->classes[i][0]) { 
			mergeClasses(lm, c, i);
		}
		if (i < lm->nLabels)  {
			free(lm->classes[i]);
			i++;
		}
	}
	lm->classes=(int **)realloc(lm->classes, lm->nClasses*sizeof(int*));
	lm->nLabels=lm->nClasses;
}

void printClasses(LabelMap *lm)
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

void freeLabelMap(LabelMap *lm)
{
	int i;
	free(lm->map);
	free(lm->labels);
	for (i=0; i < lm->nLabels; ++i)
		free(lm->classes[i]);
	free(lm->classes);
}
