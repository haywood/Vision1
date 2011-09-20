/**
 * object.h
 * Function definitions for working with object structs.
 */

#ifndef OBJECT_H_
#define OBJECT_H_

/** Fill an objectDB struct */
void makeODB(objectDB *, int);

/** Evaluate the second moment for given a, b, c, theta */
double secondMoment(int, int, int, double);

#endif /* OBJECT_H_ */
