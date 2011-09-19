#ifndef HW2_H_
#define HW2_H_

#define BINARY 1

/** Keeps track of which labels are equivalent for the sequential labeling algorithm */
typedef struct labelMap {
	int number, classes, *map;
} labelMap;

/** Get the total number of (not necessarily distinct) labels in the map */
int getNLabels(labelMap *);

/** Add a new label to the labelMap */
void addLabel(labelMap *);

/** Check if two labels are equivalent under this labelMap */
int isEquivalent(labelMap *, int, int);

/** Set two labels equivalent under this labelMap */
void setEquivalent(labelMap *, int, int);

/** Get the minimum equivalent value for a given label */
int getClass(labelMap *, int);

/** Get the number of equivalence classes of labels */
int getNClasses(labelMap *);

/** Reduce the equivalence class labels to their minimum values */
void reduceLabels(labelMap *);

/** Print the equivalence classes of the labelMap */
void printClasses(labelMap *);

/** Dellocate resources held by the labelMap */
void freeLabelMap(labelMap *);

#endif /* HW2_H_ */
