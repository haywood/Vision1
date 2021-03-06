/**
 * Object.c
 * Contains methods for the Object struct as defined in hw2.h.
 */

#include <assert.h>
#include <limits.h>
#include <float.h>

#include "hw2.h"

#define NFEATURES 4

double EPSILON[NFEATURES]={0.2, 0.3, 0.2, 0.2}; /* if two features differ by more than these fractions they will compare unequal */
typedef double featureVector[NFEATURES]; /* used in object recognition for part 4 */

/** Calculate the roundness of an object (Emin/Emax). */
float roundness(Object *o)
{
	int a=o->sm.a, b=o->sm.b, c=o->sm.c;
	return (float) secondMoment(a, b, c, o->sm.thetaMin)/secondMoment(a, b, c, o->sm.thetaMax);
}

/** Calculate the ratio of an object's area to that of its bounding box */
float rectangularity(Object *o)
{
	int width=abs(o->left - o->right), 
		 height=abs(o->top - o->bottom);
	return (float) o->area/(width*height);
}

/** Calculate the the number of holes in the object o of the image i 
 * The function attempts to reverse the idea of sequential labeling 
 * in order to find black spots on objects
 * It still needs work.
 * TODO Make this work properly
 * */
void euler(Image *im, Object *o)
{
	int i, j, k, c, neighbors[NNEIGHB], 
		 newObj, left, right;

	LabelMap lm=makeLabelMap(im);

	/* this sets up a dummy class for background darkness */
	addLabel(&lm);
	for (i=0; i < getNRows(im); ++i)
		for (j=0; j < getNCols(im); ++j)
			if (!getPixel(im, i, j)) setLabel(&lm, i, j, 1);

	for (i=o->top; i <= o->bottom; ++i) {
		left=-1;
		for (j=o->left; j <= o->right; ++j) {
			if (getPixel(im, i, j) == o->label) {
				if (left < 0) left=j; /* left edge for this row */
				right=j; /* right-most so far */
			} 
		}
		for (j=left; j <= right; ++j) {
			if (!getPixel(im, i, j)) {
				newObj=getNeighbors(&lm, i, o->top, j, left, neighbors);
				if (newObj) {
					addLabel(&lm);
					c=getNLabels(&lm);
				} else {
					for (k=0; k < NNEIGHB; ++k)
						if (neighbors[k] > 0) {
							c=evalNeighbor(k, &lm, neighbors);
							break; /* break since finding one means checking the rest */
						}
				}
				setLabel(&lm, i, j, c);
			}
		}
	}

	o->holes=getNClasses(&lm)-1; /* number is off by one because of dummy class */
	freeLabelMap(&lm);
}

double secondMoment(int a, int b, int c, double theta)
{
	return a*pow(sin(theta), 2) - b*sin(theta)*cos(theta) + c*pow(cos(theta), 2);
}

void makeODB(ObjectDB *odb, int n)
{
	SecondMoment *sm;
	Object *obj;
	int i;
	odb->objs=(Object *)malloc(n*sizeof(Object));
	odb->nObjects=n;
	for (i=0; i < n; ++i) {
		obj=odb->objs+i;
		sm=&obj->sm;

		obj->top=INT_MAX;
		obj->bottom=0;
		obj->left=INT_MAX;
		obj->right=0;
		obj->area=0;
		obj->label=i+1;
		obj->fm[0]=obj->fm[1]=0;
		obj->holes=0;

		sm->a=sm->b=sm->c=0.0;
		sm->thetaMin=sm->thetaMax=0.0;
	}
}

void getObjects(Image *im, ObjectDB *odb)
{
	int i, j, px, a, b, c, jp, ip,
		 rows=getNRows(im), cols=getNCols(im);
	float theta[2];
	double E[2];
	Object *obj;

	/* get area, calculate bounding boxes, and begin calculating centers */
	for (i=0; i < rows; ++i) {
		for (j=0; j < cols; ++j) {
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
			}
		}
	}

	/* finish centers */
	for (i=0; i < odb->nObjects; ++i)
		if (odb->objs[i].area > 0) {
			obj=odb->objs+i;
			for (j=0; j < DIM; ++j)
				obj->fm[j]/=obj->area;
		}

	/* calculate a, b, c */
	for (i=0; i < rows; ++i) {
		for (j=0; j < cols; ++j) {
			px=getPixel(im, i, j);
			if (px > 0) {
				obj=odb->objs+px-1;
				ip=i - obj->fm[0];
				jp=j - obj->fm[1];
				obj->sm.a+=jp*jp;
				obj->sm.b+=2*jp*ip;
				obj->sm.c+=ip*ip;
			}
		}
	}

	/* calculate theatmin */
	for (i=0; i < odb->nObjects; ++i)
		if (odb->objs[i].area > 0) {
			obj=odb->objs+i;
			a=obj->sm.a;
			b=obj->sm.b;
			c=obj->sm.c;

			theta[0]=0.5f*atan2(b, a-c);
			theta[1]=theta[0] + PI/2.0;

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

/* Draw the minimum second moment of every object in the database */
void drawLines(Image *im, ObjectDB *odb)
{
	double i, j;
	Object *obj;
	int k, b, h, d;

	setColors(im, getColors(im)+1); /* add new level for lines */
	for (k=0; k < odb->nObjects; ++k) {
		obj=odb->objs+k;
		b=obj->right - obj->left;
		h=obj->bottom - obj->top;
		d=sqrt(b*b+h*h)/2.0;

		i=d*sin(obj->sm.thetaMin);
		j=d*cos(obj->sm.thetaMin);

		line(im, 
				obj->fm[0] - i,
				obj->fm[1] - j,
				obj->fm[0] + i,
				obj->fm[1] + j,
				getColors(im)-1);
	}
}

/** Fill the feature vector v for the Object o */
void getFeatures(Object *o, featureVector v)
{
	v[0]=roundness(o);
	v[1]=rectangularity(o);
	v[2]=secondMoment(o->sm.a, o->sm.b, o->sm.c, o->sm.thetaMin);
	v[3]=secondMoment(o->sm.a, o->sm.b, o->sm.c, o->sm.thetaMax);
}

/** Euclidean metric of two feature vectors */
int featureCmp(featureVector v1, featureVector v2)
{
	double r;
	int i;
	for (i=0; i < NFEATURES; ++i) {
		r=v1[i]/v2[i];
		if (fabs(1.0 - r) > EPSILON[i]) {
			return 0;
		}
	}
	return 1;
}

/** Compare objects based on the definition of a feature vector as calculated in getFeatures. 
 * Take the closest sufficiently close test object to each known one
 * */
int recognize(ObjectDB *test, ObjectDB *known)
{
	featureVector tv, kv;
	int i, j;

	for (i=0; i < test->nObjects; ++i) {
		test->objs[i].label=0;
	}
	for (i=0; i < known->nObjects; ++i) {
		getFeatures(known->objs+i, kv);
		for (j=0; j < test->nObjects; ++j) {
			getFeatures(test->objs+j, tv);
			if (featureCmp(tv, kv)) {
				test->objs[j].label=known->objs[i].label;
			}
		}
	}
	return 0;
}

void readDatabase(ObjectDB *odb, const char *dbname)
{
	FILE *f=fopen(dbname, "r");
	float eMin, rndnss, inDeg;
	int i=0, n, nFields=15;
	char line[1024];
	Object *o;

	while (!feof(f)) {
		fscanf(f, "%d", &odb->nObjects);
		fgets(line, sizeof line, f);
	}
	
	makeODB(odb, odb->nObjects);
	
	if (fseek(f, 0, SEEK_SET)) {
		fprintf(stderr, "error reading database file\n");
		exit(1);
	}

	while (i < odb->nObjects) {
		o=odb->objs+(i++);
		if ((n=fscanf(f, "%d %d %d %f %f %f %d %d %d %d %d %d %d %d %d",
					&o->label,
					o->fm, o->fm+1,
					&eMin,
					&inDeg,
					&rndnss,
					&o->area,
					&o->holes,
					&o->sm.a, &o->sm.b, &o->sm.c,
					&o->top, &o->bottom, &o->left, &o->right)) != nFields) {
			fprintf(stderr, "bad database file. got %d fields instead of %d\n", n, nFields);
			exit(1);
		}
		o->sm.thetaMin= PI/2 - inDeg/DEG_PER_RAD;
		o->sm.thetaMax= (float) PI/2 + o->sm.thetaMin;

		fgets(line, sizeof line, f);
	}
	fclose(f);
}

void writeObject(FILE *f, Object *o)
{
	double eMin=secondMoment(o->sm.a, o->sm.b, o->sm.c, o->sm.thetaMin);
	
	fprintf(f, "%d %d %d %f %f %f %d %d %d %d %d %d %d %d %d\n",
			o->label, 
			o->fm[0], o->fm[1],
			eMin,
			DEG_PER_RAD*(PI/2 - o->sm.thetaMin),
			roundness(o),
			o->area,
			o->holes,
			o->sm.a, o->sm.b, o->sm.c,
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
