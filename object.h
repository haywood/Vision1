/**
 * Object.h
 * Function definitions for working with Object structs.
 */

#ifndef OBJECT_H_
#define OBJECT_H_

/** Calculate the roundness of an object (Emin/Emax) */
float roundness(Object *);

/** Calculate the ratio of an object's area to that of its bounding box */
float rectangularity(Object *);

/** Calculate an objects Euler number */
void euler(Image *, Object *);

/** Evaluate the second moment for given a, b, c, theta */
double secondMoment(int, int, int, double);

/** Draw the minium axes for the given ObjectDB on the given Image */
void drawLines(Image *, ObjectDB *);

/** Fill an ObjectDB struct */
void makeODB(ObjectDB *, int);

/** Fill the given ObjectDB with objects from the given image. */
void getObjects(Image *, ObjectDB *);

/** Test the given Object for recognition against the ObjectDB */
int recognize(ObjectDB *, ObjectDB *);

/** Read a file into an ObjectDB struct */
void readDatabase(ObjectDB *, const char *);

/** Write the ObjectDB to the file with given name */
void writeDatabase(ObjectDB *, const char *);

#endif /* OBJECT_H_ */
