#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vision_utilities.h"

int setSize(Image *im, int rows, int columns) {
  im->rows=rows;
  im->columns=columns;
  im->data = (uchar *) malloc(rows * columns * sizeof(char));
  if(im->data == NULL) {
       fprintf(stderr, "Insufficient memory for new image\n");
        exit(1); 
        } 

  return 0;
}

int setSizeColor(ImageColor *im, int rows, int columns) {
  im->rows=rows;
  im->columns=columns;
  im->dataR = (uchar *) malloc(rows * columns * sizeof(char));
  if(im->dataR == NULL) {
       fprintf(stderr, "Insufficient memory for new image\n");
	exit(1); 
        }
  
  im->dataG = (uchar *) malloc(rows * columns * sizeof(char));
  if(im->dataG == NULL) {
       fprintf(stderr, "Insufficient memory for new image\n");
	exit(1); 
        }
  
  im->dataB = (uchar *) malloc(rows * columns * sizeof(char));
  if(im->dataB == NULL) {
       fprintf(stderr, "Insufficient memory for new image\n");
	exit(1); 
        } 

  return 0;
}

int getNCols(const Image *im) {
  return im->columns;
}

int getNColsColor(const ImageColor *im) {
  return im->columns;
}

int getNRows(const Image *im) {
  return im->rows;
}

int getNRowsColor(const ImageColor *im) {
  return im->rows;
}

int setColors(Image *im, int colors) {
  im->colors=colors;
  return 0;
}

int setColorsColor(ImageColor *im, int colors) {
  im->colors=colors;
  return 0;
}

int getColors(const Image *im) {
  return im->colors;
}

int getColorsColor(const ImageColor *im) {
  return im->colors;
}

int setPixel(Image *im, int i, int j, int color) {
  i=CLAMP(i, 0, im->rows-1);
  j=CLAMP(j, 0, im->columns-1);
  im->data[i*(im->columns)+j]=color;
  return 0;
}

int setPixelColor(ImageColor *im, int i, int j, int colorR, int colorG, int colorB) {
  i=CLAMP(i, 0, im->rows-1);
  j=CLAMP(j, 0, im->columns-1);
  im->dataR[i*(im->columns)+j]=colorR;
  im->dataG[i*(im->columns)+j]=colorG;
  im->dataB[i*(im->columns)+j]=colorB;
  return 0;
}

int getPixel(const Image *im, int i, int j) {
  i=CLAMP(i, 0, im->rows-1);
  j=CLAMP(j, 0, im->columns-1);
  return (int) im->data[i*(im->columns)+j];
}

int getPixelColor(const ImageColor *im, int i, int j, int colorNum) {
  i=CLAMP(i, 0, im->rows-1);
  j=CLAMP(j, 0, im->columns-1);
  if (colorNum==1)  return (int) im->dataR[i*(im->columns)+j];
  if (colorNum==2)  return (int) im->dataG[i*(im->columns)+j];
  if (colorNum==3)  return (int) im->dataB[i*(im->columns)+j];
  return -1;
}

int readImage(Image *im, const char *fname) {  
  FILE *input;
  char line[1024];     
  int nCols,nRows;  
  int levels;
  int i, j;

  /* open it */
  if (!fname || (input=fopen(fname,"rb"))==0)
    return-1;

  /* check for the right "magic number" */
  if (
        fread(line,1,3,input)!=3
      ||strncmp(line,"P5\n",3)
     )
  {
    fclose(input);
    return -1;
  }
  
  /* skip the comments */
  do
    fgets(line,sizeof line,input);
  while(*line=='#');

  /* read the width and height */
  sscanf(line,"%d %d\n",&nCols,&nRows);
  setSize(im, nRows, nCols);

  /* read # of gray levels */
  fgets(line,sizeof line,input);
  sscanf(line,"%d\n",&levels);
  setColors(im, levels);
  
  /* read pixel row by row */
  for(i=0;i<nRows;i++)
  {
    for(j=0;j<nCols;j++)
    {
      int byte=fgetc(input);

      if (byte==EOF) /* short file */
      {
        fclose(input);
        return -1;
      }
      else
        setPixel(im,i,j,byte);	
    }
  }

  /* close the file */
  fclose(input);
  return 0; /* OK */
}

int readImageColor(ImageColor *im, const char *fname) {  
  FILE *input;
  char line[1024];     
  int nCols,nRows;  
  int levels;
  int i, j;

  /* open it */
  if (!fname || (input=fopen(fname,"rb"))==0)
    return-1;

  /* check for the right "magic number" */
  if (
        fread(line,1,3,input)!=3
      ||strncmp(line,"P6\n",3)
     )
  {
    fclose(input);
    return -1;
  }
  
  /* skip the comments */
  do
    fgets(line,sizeof line,input);
  while(*line=='#');

  /* read the width and height */
  sscanf(line,"%d %d\n",&nCols,&nRows);
  setSizeColor(im, nRows, nCols);
 
  /* read # of gray levels */
  fgets(line,sizeof line,input);
  sscanf(line,"%d\n",&levels);
  setColorsColor(im, levels);
  
  /* read pixel row by row */
  for(i=0;i<nRows;i++)
  {
    for(j=0;j<nCols;j++)
    {
      int byteR, byteG, byteB;
      byteR=fgetc(input);
      if (byteR==EOF) /* short file */
      {
        fclose(input);
        return -1;
      }
      byteG=fgetc(input);
      if (byteG==EOF) /* short file */
      {
        fclose(input);
        return -1;
      }
      byteB=fgetc(input);
      if (byteB==EOF) /* short file */
      {
        fclose(input);
        return -1;
      }
      else
        setPixelColor(im,i,j, byteR, byteG, byteB);	
    }
  }

  /* close the file */
  fclose(input);
  return 0; /* OK */
}

int writeImage(const Image *im, const char *fname) {
  FILE *output; 
  int nRows;
  int nCols;
  int colors;
  int i, j;
    
  /* open the file */
  if (!fname || (output=fopen(fname,"wb"))==0)
    return(-1);

  nRows=getNRows(im);
  nCols=getNCols(im);
  colors=getColors(im);

  /* write the header */
  fprintf(output,"P5\n"); /* magic number */
  fprintf(output,"#\n");  /* empty comment */
  fprintf(output,"%d %d\n%03d\n",nCols,nRows,colors); /* image info */

  /* write pixels row by row */
  for(i=0;i<nRows;i++)
  {
    for(j=0;j<nCols;j++)
    {
      int byte=getPixel(im,i,j);

      if (fputc(byte,output)==EOF) /* couldn't write */
      {
	    fclose(output);
	    return -1;
      }      
    }
  }
  
  /* close the file */
  fclose(output);
  return 0; /* OK */
}

int writeImageColor(const ImageColor *im, const char *fname) {
  FILE *output; 
  int nRows;
  int nCols;
  int colors;
  int i, j;
  
  /* open the file */
  if (!fname || (output=fopen(fname, "wb"))==0)
    return(-1);

  nRows=getNRowsColor(im);
  nCols=getNColsColor(im);
  colors=getColorsColor(im);

  /* write the header */
  fprintf(output,"P6\n"); /* magic number */
  fprintf(output,"#\n");  /* empty comment */
  fprintf(output,"%d %d\n%03d\n",nCols,nRows,colors); /* image info */

  /* write pixels row by row */
  for(i=0;i<nRows;i++)
    {
      for(j=0;j<nCols;j++)
        {
          int byteR, byteG, byteB;
          byteR=getPixelColor(im,i,j, 1);
          if (fputc(byteR,output)==EOF) /* couldn't write */
            {
              fclose(output);
              return -1;
            }      
          byteG=getPixelColor(im,i,j, 2);
          if (fputc(byteG,output)==EOF) /* couldn't write */
            {
              fclose(output);
              return -1;
            }      
          byteB=getPixelColor(im,i,j, 3);
          if (fputc(byteB,output)==EOF) /* couldn't write */
            {
              fclose(output);
              return -1;
            }      
        }
    }

  /* close the file */
  if (fclose(output)==EOF) return -1;
  return 0; /* OK */
}

int line(Image *im, int y0, int x0, int y1, int x1, int color) {  
  int xmin,xmax; /* line coordinates */
  int ymin,ymax;
  int dir;       /* scan direction */
  int dx;        /* distance along X */
  int dy;        /* distance along Y */

  /* increments: East, North-East, South, South-East, North */
  int incrE,
      incrNE,
      incrSE;
  int d;         /* the D */
  int x,y;       /* running coordinates */
  int mpCase;    /* midpoint algorithm's case */
  int done;      /* set to 1 when done */

  /* TODO note that the x and y are switched! */
  xmin=y0;
  xmax=y1;
  ymin=x0;
  ymax=x1;
  
  dx=xmax-xmin;
  dy=ymax-ymin;

  if (dx*dx>dy*dy) /* horizontal scan */
  {
    dir=DIR_X;
    if (xmax<xmin)
    {
      SWAP(xmin,xmax);
      SWAP(ymin,ymax);
    }
    dx=xmax-xmin;
    dy=ymax-ymin;

    if (dy>=0)
    {
      mpCase=1;
      d=2*dy-dx;      
    }
    else
    {
      mpCase=2;
      d=2*dy+dx;      
    }

    incrNE=2*(dy-dx);
    incrE=2*dy;
    incrSE=2*(dy+dx);
  }
  else /* vertical scan */
  {
    dir=DIR_Y;
    if (ymax<ymin)
    {
      SWAP(xmin,xmax);
      SWAP(ymin,ymax);
    }
    dx=xmax-xmin;
    dy=ymax-ymin;    

    if (dx>=0)
    {
      mpCase=1;
      d=2*dx-dy;      
    }
    else
    {
      mpCase=2;
      d=2*dx+dy;      
    }

    incrNE=2*(dx-dy);
    incrE=2*dx;
    incrSE=2*(dx+dy);
  }
  
  /* start the scan */
  x=xmin;
  y=ymin;
  done=0;

  while(!done)
  {
    setPixel(im,x,y,color);
  
    /* move to the next point */
    switch(dir)
    {
      case DIR_X: /* horizontal */
      {
        if (x<xmax)
        {
	      switch(mpCase)
          {
            case 1:
	          if (d<=0)
	          {
	            d+=incrE;  x++;
	          }
              else
              {
                d+=incrNE; x++; y++;
              }
            break;
  
            case 2:
              if (d<=0)
              {
                d+=incrSE; x++; y--;
              }
              else
              {
                d+=incrE;  x++;
              }
            break;
	      } /* mpCase */
        } /* x<xmax */
        else
	      done=1;
      }  
      break;

      case DIR_Y: /* vertical */
      {
        if (y<ymax)
        {
          switch(mpCase)
          {
            case 1:
              if (d<=0)
              {
                d+=incrE;  y++;
              }
              else
              {
                d+=incrNE; y++; x++;
              }
            break;
  
            case 2:
              if (d<=0)
              {
                d+=incrSE; y++; x--;
              }
              else
              {
                d+=incrE;  y++;
              }
            break;
	      } /* mpCase */
        } /* y<ymin */
        else
	      done=1;
      }
      break;    
    }
  }

  return 0; /* no error */
}
