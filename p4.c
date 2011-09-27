/**
 * p4.c
 * A program to recognize and label objects in an image against those in a labeled database.
 */

#include "hw2.h"

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
	readDatabase(&known, idb);
	makeODB(&newobjs, getColors(&im));
	getObjects(&im, &newobjs);
	drawLines(&im, &newobjs);
	writeImage(&im, oim);

	free(newobjs.objs);
	free(known.objs);
	free(im.data);

	return 0;
}
