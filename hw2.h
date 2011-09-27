/**
 * hw2.h
 * Contains useful typedefs and macros.
 */

#ifndef HW2_H_
#define HW2_H_

#define BINARY 1
#define DIM 2
#define PIXEL_MAX 255
#define DEG_PER_RAD (180.0f/PI)

#include <stdlib.h>
#include <stdio.h>

#include "vision_utilities.h"

/** Keeps track of which labels are equivalent for the sequential labeling algorithm */
typedef struct LabelMap {
	int nLabels, nClasses;
	int *map, *labels, **classes;
	Image *im;
} LabelMap;

#include "LabelMap.h"

/** Holds an Object's second moment */
typedef struct SecondMoment {
	int a, b, c;
	float thetaMin, thetaMax;
} SecondMoment;

/** Holds data on Objects found in an image */
typedef struct Object {
	int label;
	int fm[DIM];
	SecondMoment sm;
	int top, bottom, left, right;
	int area;
} Object;

typedef struct ObjectDB {
	Object *objs;
   int nObjects;
} ObjectDB;

#include "Object.h"

#endif /* HW2_H_ */
