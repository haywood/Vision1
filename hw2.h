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
#include <string.h>
#include <stdio.h>

#include "vision_utilities.h"

/** Keeps track of which labels are equivalent for the sequential labeling algorithm */
typedef struct LabelMap {
	int nLabels, nClasses;
	int *map, /* map from labels to classes */
		 *labels, /* an ints array for storing label by pixel. it uses ints because the chars overflow when this information is stored in the Image. */
		 **classes; /* a mapping from classes to labels */
	Image *im; /* the Image to which this LabelMap is relevant */
} LabelMap;

#include "LabelMap.h"

/** Holds an Object's second moment */
typedef struct SecondMoment {
	int a, b, c;
	float thetaMin, thetaMax;
} SecondMoment;

/** Holds data on Objects found in an image */
typedef struct Object {
	int label; /* Object label in its corresponding image */
	int fm[DIM]; /* first moment  (row, col) */
	SecondMoment sm; /* second moment */
	int top, bottom, left, right; /* edeges of bounding box */
	int area; /* area */
	int holes; /* number of holes in the object */
} Object;

typedef struct ObjectDB {
	Object *objs; /* An array of Object structs */
   int nObjects; /* The number of Objects */
} ObjectDB;

#include "Object.h"

#endif /* HW2_H_ */
