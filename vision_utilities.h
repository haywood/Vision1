#include <math.h>
#include <sys/time.h>


#define CLAMP(A,L,H)       ((A)<=(L) ? (L) : (A)<=(H) ? (A) : (H))
#define MAX(A,B)   ((A) > (B) ? (A) : (B))
#define MIN(A,B)   ((A) < (B) ? (A) : (B))

/** Macro to swap 2 variables bit by bit */
#ifdef SWAP
#undef SWAP
#endif
#define SWAP(a,b) {a^=b; b^=a; a^=b;}

#define PI 3.14159

/** A grayscale image */
typedef struct _image {
  int rows;
  int columns;
  int colors;
  unsigned char * data;
} Image;

/** A color image with upto 3 channels */
typedef struct _imageColor {
  int rows;
  int columns;
  int colors;
  unsigned char * dataR;
  unsigned char * dataG;
  unsigned char * dataB;
} ImageColor;

/** A floating point kernel for applying convolutions */
typedef struct _kernel {
  int width;    /*width of kernel */
  int height;   /*height of kernel*/
  float *kernel;/*pointer to floating point kernel*/
} Kernel, *KernelP;

/** Simple typedef for an unsigned character (8-bit values from 0-255) */
typedef unsigned char uchar;

/** A floating point coordinate */
typedef struct _pixelAr {
  float xCoor;
  float yCoor;
} PixelAr;

/** Scan directions */
enum {
  DIR_X,
  DIR_Y
};


/** Sets the size of the given grayscale image.
 *  The size is a given 
 *  height (# of rows) and width (# of columns).
 *  returns 0 if OK or -1 if fails.
 */
int setSize(Image *im, int rows, int columns);

/** Sets the size of the given color image.
 *  The size is a given 
 *  height (# of rows) and width (# of columns).
 *  returns 0 if OK or -1 if fails.
 */
int setSizeColor(ImageColor *im, int rows, int columns);

/** Returns the number of columns in the given grayscale image. */
int getNCols(const Image *im);

/** Returns the number of columns in the given color image.  */
int getNColsColor(const ImageColor *im);


/** Returns the number of rows in the given grayscale image. */
int getNRows(const Image *im);

/** Returns the number of rows in the given color image. */
int getNRowsColor(const ImageColor *im);

/** Sets the number of gray-levels in the image (not counting 0).
 *  Returns 0 if OK or -1 if fails;
 */
int setColors(Image *im, int colors);

/** Sets the number of gray-level colors in the image (for all channels, not counting 0).
 *  Returns 0 if OK or -1 if fails.
 */
int setColorsColor(ImageColor *im, int colors);

/** Returns the number of gray-levels in the grayscale image. */
int getColors(const Image *im);
 
/** Returns the number of gray-levels in the color image. */
int getColorsColor(const ImageColor *im);
   
/** Sets the pixel in the grayscale image at row i and column j to the given graylevel. */
int setPixel(Image *im, int i, int j, int graylevel);

/** Sets the pixel in the color image at row i and column j to the given color. */
int setPixelColor(ImageColor *im, int i, int j, int colorR, int colorG, int colorB);

/** Returns the graylevel of the pixel in the grayscale image at row i and column j. */	
int getPixel(const Image *im, int i, int j);

/** Returns the color of the pixel in the color image at row i and column j and with color colorNum. */	
int getPixelColor(const ImageColor *im, int i, int j, int colorNum);

/** Reads grayscale image from fname.
 *  Returns 0 if OK or -1 if something goes wrong.
 */
int readImage(Image *im, const char *fname);

/** Reads color image from fname.
 *  Returns 0 if OK or -1 if something goes wrong.
 */
int readImageColor(ImageColor *im, const char *fname);

/** Writes the grayscale image in portable greymap (PGM) format to fname.
 *  returns 0 if OK or -1 if something goes wrong.
 */
int writeImage(const Image *im, const char *fname);

/** Writes the color image to fname.
 *  Returns 0 if OK or -1 if something goes wrong.
 */
int writeImageColor(const ImageColor *im, const char *fname);

/** Draws a line of given gray-level color from (x0,y0) to (x1,y1).
 *  im is the pointer to the user defined image structure - 
 *  whatever it is.
 *
 *  Returns 0.    
 *
 *  Uses the function setPixel(Image *im, int x, int y, int color)
 *  to actually set pixels in the image to the given color. 
 *
 *  IMPORTANT: (x0,y0) and (x1,y1) can lie outside the image 
 *  boundaries, so setPixel() should check the coordinates passed to it.
 *
 *  Implements the Bresenham's incremental midpoint algorithm.
 *  (adapted from J.D.Foley, A. van Dam, S.K.Feiner, J.F.Hughes
 *  "Computer Graphics. Principles and practice", 
 *  2nd ed., 1990, section 3.2.2).  
 *
 *  Copyright: Anton Nikolaev, 1995
 *  NO WARRANTIES: use at your own risk
 */
int line(Image *im, int y0, int x0, int y1, int x1, int color);
