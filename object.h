/**
 * Object.h
 * Function definitions for working with Object structs.
 */

#ifndef OBJECT_H_
#define OBJECT_H_

/** Evaluate the second moment for given a, b, c, theta */
double secondMoment(int, int, int, double);

/** Draw the minium axes for the given ObjectDB on the given Image */
void drawLines(Image *, ObjectDB *);

/** Fill an ObjectDB struct */
void makeODB(ObjectDB *, int);

/** Fill the given ObjectDB with objects from the given image. */
void getObjects(Image *, ObjectDB *);

#endif /* OBJECT_H_ */
