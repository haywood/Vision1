/**
 * p3.c
 * A program to take a labeled image file and output the corresponding object database file and another labeled image with lines along each object's axis of minimum inertia.
 */

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <stdio.h>

#include "hw2.h"

int main(int argc, char *argv[])
{
	char *ifname, *odname, *ofname;
	ObjectDB odb;
	Image im;

	if (argc < 4) {
		fprintf(stderr, "usage: %s <input labeled image> <output database> <output image>", argv[0]);
		return 1;
	}

	ifname=argv[1];
	odname=argv[2];
	ofname=argv[3];

	readImage(&im, ifname);
	makeODB(&odb, getColors(&im));
	getObjects(&im, &odb);
	writeDatabase(&odb, odname);
	drawLines(&im, &odb);
	writeImage(&im, ofname);

	free(odb.objs);
	free(im.data);

	return 0;
}
