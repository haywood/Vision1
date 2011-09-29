/**
 * p4.c
 * A program to recognize and label objects in an image against those in a labeled database.
 */

#include "hw2.h"

void filterObjects(Image *, ObjectDB *, ObjectDB *);

int main(int argc, char *argv[])
{
	char *iim, *idb, * oim;
	ObjectDB newobjs, known;
	Image im;

	if (argc != 4) {
		fprintf(stderr, "usage: %s <input labeled image> <input database> <output image>", argv[0]);
	}

	iim=argv[1];
	idb=argv[2];
	oim=argv[3];

	readImage(&im, iim);
	readDatabase(&known, idb); /* get the database of known objects */
	makeODB(&newobjs, getColors(&im)); /* create the database for the input image */
	getObjects(&im, &newobjs); /* fill the database for the input image */
	filterObjects(&im, &newobjs, &known); /* find known objects in the database and throw away those unknown */
	drawLines(&im, &newobjs); /* draw lines in the input image */
	writeImage(&im, oim); /* write the output image */

	free(newobjs.objs);
	free(known.objs);
	free(im.data);

	return 0;
}

void filterObjects(Image *im, ObjectDB *test, ObjectDB *known)
{
	int i, j, px, n=0;

	recognize(test, known);

	for (i=0; i < test->nObjects; ++i) {
		if (test->objs[i].label) {
			n++;
		}
	}
	for (i=0; i < getNRows(im); ++i) {
		for (j=0; j < getNCols(im); ++j) {
			px=getPixel(im, i, j);
			if ((px > 0) && !test->objs[px-1].label) {
					setPixel(im, i, j, 0);
			}
		}
	}

	i=0;
	for (j=n; j < test->nObjects && i < n; ++j) {
		while (i < n && test->objs[i].label) i++;
		if (test->objs[j].label) {
			memcpy(test->objs+i, test->objs+j, sizeof(Object));
		}
	}

	test->objs=(Object *)realloc(test->objs, n*sizeof(Object));
	if (test->objs == NULL) {
		fprintf(stderr, "ran out of memory while filtering objects.\n");
		exit(1);
	}
	test->nObjects=n;
}
