/**
 * Object.c
 * Contains methods for the Object struct as defined in hw2.h.
 */

#include <assert.h>
#include <limits.h>
#include <float.h>

#include "hw2.h"

#define NFEATURES 3

typedef float featureVector[NFEATURES];

float roundness(Object *o)
{
	int a=o->sm.a, b=o->sm.b, c=o->sm.c;
	return (float) secondMoment(a, b, c, o->sm.thetaMin)/secondMoment(a, b, c, o->sm.thetaMax);
}

float rectangularity(Object *o)
{
	int width=abs(o->left - o->right), 
		 height=abs(o->top - o->bottom);
	return (float) o->area/(width*height);
}

void euler(Image *im, Object *o)
{
	int i=o->top, j=o->left, k, c, 
		 neighbors[NNEIGHB], newObj;
	LabelMap lm=makeLabelMap(im);

	for (;i <= o->bottom; ++i) {
		for (;j <= o->right; ++j) {
			if (!getPixel(im, i, j)) {
				newObj=getNeighbors(&lm, i, o->top, j, o->left, neighbors);
				if (newObj) {
					addLabel(&lm);
					c=getNLabels(&lm);
				} else {
					for (k=0; k < NNEIGHB; ++k)
						if (neighbors[k]) {
							c=evalNeighbor(k, &lm, neighbors);
							break; /* break since finding one means checking the rest */
						}
				}
				setLabel(&lm, i, j, c);
			}
		}
	}

	o->eulerNum=getNClasses(&lm);
	printf("%d\n", o->eulerNum);
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
		obj->eulerNum=0;

		sm->a=sm->b=sm->c=0.0;
		sm->thetaMin=sm->thetaMax=0.0;
	}
}

void getObjects(Image *im, ObjectDB *odb)
{
	int i, j, px, a, b, c,
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
				obj->sm.a+=j*j;
				obj->sm.b+=j*i;
				obj->sm.c+=i*i;
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
			obj->sm.a-=obj->fm[1]*obj->fm[1]*obj->area;
			obj->sm.b-=obj->fm[1]*obj->fm[0]*obj->area;
			obj->sm.b*=2;
			obj->sm.c-=obj->fm[0]*obj->fm[0]*obj->area;

			a=obj->sm.a;
			b=obj->sm.b;
			c=obj->sm.c;

			theta[0]=0.5f*atan((float)b/(a-c));
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
			euler(im, odb->objs+i);
		}
}

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

void getFeatures(Object *o, featureVector v)
{
	v[0]=roundness(o);
	v[1]=rectangularity(o);
	v[2]=o->eulerNum;
}

int featureCmp(featureVector v1, featureVector v2)
{
	int i;
	for (i=0; i < NFEATURES; ++i) {
		if ((v1[i] - v2[i]) > FLT_EPSILON) {
			return 0;
		}
	}
	return 1;
}

int recognize(Object *test, ObjectDB *known)
{
	featureVector tv, kv;
	int i;

	getFeatures(test, tv);
	for (i=0; i < known->nObjects; ++i) {
		getFeatures(known->objs+i, kv);
		if (featureCmp(tv, kv)) {
			return 1;
		}
	}
	return 0;
}

void readDatabase(ObjectDB *odb, const char *dbname)
{
	FILE *f=fopen(dbname, "r");
	float eMin, rndnss, inDeg;
	int i=0, n, nFields=14;
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
		if ((n=fscanf(f, "%d %d %d %f %f %f %d %d %d %d %d %d %d %d",
					&o->label,
					o->fm, o->fm+1,
					&eMin,
					&inDeg,
					&rndnss,
					&o->area,
					&o->sm.a, &o->sm.b, &o->sm.c,
					&o->top, &o->bottom, &o->left, &o->right)) != nFields) {
			fprintf(stderr, "bad database file. got %d fields instead of %d\n", n, nFields);
			exit(1);
		}
		o->sm.thetaMin= PI - inDeg/DEG_PER_RAD;
		o->sm.thetaMax= (float) PI/2 + o->sm.thetaMin;

		fgets(line, sizeof line, f);
	}
	fclose(f);
}

void writeObject(FILE *f, Object *o)
{
	double eMin=secondMoment(o->sm.a, o->sm.b, o->sm.c, o->sm.thetaMin);
	
	fprintf(f, "%d %d %d %f %f %f %d %d %d %d %d %d %d %d\n",
			o->label, 
			o->fm[0], o->fm[1],
			eMin,
			DEG_PER_RAD*(PI - o->sm.thetaMin),
			roundness(o),
			o->area,
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
