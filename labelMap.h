/**
 * LabelMap.h
 * Function definitions for working with LabelMap structs.
 */

#ifndef LABELMAP_H_
#define LABELMAP_H_

#define NNEIGHB 3
enum {
	WEST,
	NORTHWEST,
	NORTH
};

/** Make a new LabelMap for the given Image */
LabelMap makeLabelMap(Image *);

/** Get the label at a given pixel */
int getLabel(LabelMap *, int, int);

/** Set the label at a given pixel */
void setLabel(LabelMap *, int, int, int);

/** Label a pixel under the sequential labeling algorithm */
void labelPixel(LabelMap *, int, int);

/** Get the labels of a pixel's neighbors */
int getNeighbors(LabelMap *, int, int, int, int, int[NNEIGHB]);

/** Evaluate a particular neighbor */
int evalNeighbor(int, LabelMap *, int[NNEIGHB]);

/** Evaluate a given label from an array of neighbors */

/** Resolve the label at a given pixel to its equivalence class */
void resolveLabel(LabelMap *, int, int);

/** Get a pointer to the Image to which this LabelMap applies */
Image *getImage(LabelMap *);

/** Get the total number of (not necessarily distinct) labels in the map */
int getNLabels(LabelMap *);

/** Add a new label to the LabelMap */
void addLabel(LabelMap *);

/** Check if two labels are equivalent under this LabelMap */
int isEquivalent(LabelMap *, int, int);

/** Set two labels equivalent under this LabelMap */
void setEquivalent(LabelMap *, int, int);

/** Get the minimum equivalent value for a given label */
int getClass(LabelMap *, int);

/** Get the number of equivalence classes of labels */
int getNClasses(LabelMap *);

/** Reduce the equivalence class labels to their minimum values */
void reduceLabels(LabelMap *);

/** Print the equivalence classes of the LabelMap */
void printClasses(LabelMap *);

/** Dellocate resources held by the LabelMap */
void freeLabelMap(LabelMap *);

#endif /* LABELMAP_H_ */
