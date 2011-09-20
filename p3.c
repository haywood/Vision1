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

void getObjects(Image *, ObjectDB *);
void writeDatabase(ObjectDB *, const char *);
void drawLines(Image *, ObjectDB *);

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

void getObjects(Image *im, ObjectDB *odb)
{
	int i, j, px, 
		 a, b, c;
	double theta[2];
	double E[2];
	Object *obj;

	/* get area, calculate bounding boxes, and begin calculating centers */
	for (i=0; i < getNRows(im); ++i) {
		for (j=0; j < getNRows(im); ++j) {
			px=getPixel(im, i, j);
			if (px > 0) {
				obj=odb->objs+px-1;
				if (i < obj->top) obj->top=i;
				if (i > obj->bottom) obj->bottom=i;
				if (j < obj->left) obj->left=j;
				if (j > obj->right) obj->right=j;
				obj->fm[0]+=i;
				obj->fm[1]+=j;
				obj->area++;
				obj->sm.a+=i*i;
				obj->sm.b+=i*j;
				obj->sm.c+=j*j;
			}
		}
	}

	/* finish centers and second moment coefficients */
	for (i=0; i < odb->nObjects; ++i)
		if (odb->objs[i].area > 0) {
			obj=odb->objs+i;
			for (j=0; j < DIM; ++j)
				obj->fm[j]/=obj->area;

			/* TODO Explain this reduction */
			obj->sm.a-=obj->fm[0]*obj->fm[0]*obj->area;
			obj->sm.b-=obj->fm[0]*obj->fm[1]*obj->area;
			obj->sm.b*=2;
			obj->sm.c-=obj->fm[1]*obj->fm[1]*obj->area;

			a=obj->sm.a;
			b=obj->sm.b;
			c=obj->sm.c;

			theta[0]=0.5*atan((double)b/a-c);
			theta[1]=fabs(theta[0] + PI/2.0);

			E[0]=secondMoment(a, b, c, theta[0]);
			E[1]=secondMoment(a, b, c, theta[1]);

			if (E[0] > E[1]) {
				obj->sm.thetaMin=theta[1];
				obj->sm.thetaMax=theta[0];
			} else {
				obj->sm.thetaMin=theta[0];
				obj->sm.thetaMax=theta[1];
			}
		}
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
			DEG_PER_RAD*fmod(PI - o->sm.thetaMin, PI),
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

void drawLines(Image *im, ObjectDB *odb)
{
	double h, v, m, n;
	Object *obj;
	int i;
	for (i=0; i < odb->nObjects; ++i) {
		obj=odb->objs+i;
		m=obj->right - obj->left;
		n=obj->bottom - obj->top;
		if (m>n) n=m;
		h=n*cos(obj->sm.thetaMin);
		v=n*sin(obj->sm.thetaMin);
		line(im, 
				obj->fm[0] - v,
				obj->fm[1] - h,
				obj->fm[0] + v,
				obj->fm[1] + h,
				i);
	}
}
