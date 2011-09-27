/**
 * p3.c
 */

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <stdio.h>

#include "hw2.h"

void writeDatabase(ObjectDB *, const char *);

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

void writeObject(FILE *f, Object *o)
{
	double eMin=secondMoment(o->sm.a, o->sm.b, o->sm.c, o->sm.thetaMin),
			 eMax=secondMoment(o->sm.a, o->sm.b, o->sm.c, o->sm.thetaMax);
	
	/* label center-row center-col Emin thetaMin roundness area/bounding-area top bottom left right*/
	fprintf(f, "%d %d %d %f %f %f %f %d %d %d %d\n",
			o->label, 
			o->fm[0], o->fm[1],
			eMin,
			DEG_PER_RAD*(PI - o->sm.thetaMin),
			eMin/eMax,
			(double)o->area/((o->right-o->left)*(o->bottom-o->top)),
			o->top, o->bottom, o->left, o->right);
}

void writeDatabase(ObjectDB *odb, const char *dbname)
{
	FILE *f=fopen(dbname, "w");
	int i;
	if (f == NULL) {
		fprintf(stderr, "unable to write database\n");
		return;
	}
	for (i=0; i < odb->nObjects; ++i)
		writeObject(f, &odb->objs[i]);
	fclose(f);
}
