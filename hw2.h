/**
 * hw2.h
 * Contains useful typedefs and macros.
 */
#ifndef HW2_H_
#define HW2_H_

#define BINARY 1
#define DIM 2
#define PIXEL_MAX 255

#include "vision_utilities.h"

/** Keeps track of which labels are equivalent for the sequential labeling algorithm */
typedef struct labelMap {
	int nLabels, nClasses;
	int *map, *labels, **classes;
	Image *im;
} labelMap;

#include "labelMap.h"

/** Holds an object's second moment */
typedef struct SecondMoment {
	int a, b, c;
	double thetaMin, thetaMax;
} SecondMoment;

/** Holds data on objects found in an image */
typedef struct object {
	int label;
	int fm[DIM];
	SecondMoment sm;
	int top, bottom, left, right;
	int area;
} object;

typedef struct objectDB {
	object *objs;
   int nObjects;
} objectDB;

#include "object.h"

#endif /* HW2_H_ */
