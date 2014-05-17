// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class IP
//! \brief Image Processing class
//!
//! \file ip.cpp
//! \brief Image Processing class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include	"ip.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief constructor
IP::IP()
{	// init lut with level 0
	lookUpTable(0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Look up table
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief look up table for thresholding
//! \details everything below the level is 0; 255 otherwise
//! \param[in] thresLevel	threshold level
// everything below the level is 0; 255 otherwise
void IP::lookUpTable(int thresLevel)
{
	lut	= new int[256];
	int	i;

	for (i = 0; i < thresLevel; i++)
		lut[i]	= 0;
	for (	  ; i < 256; i++)
		lut[i]	= 255;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// process image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief process image (point thresholding)
//! \details process image based on funct
//! \param[in] funct	enum; process function
//! \param[in, out] img	address of the image to be process
// process image based on funct
void IP::processImg(IP_FUNCT funct, QImage &img)
{
	int col;
	int width 	= img.width();
	int height	= img.height();

	for (int y = 0; y < height; y++)
	{
		unsigned char* pixData	= img.scanLine(y);

		for (int x = 0; x < width; x++)
		{
			switch(funct)
			{
				case Red:
				{
					col 		= *pixData+2;
					*pixData++	= col;		// Blue channel
					*pixData++	= col;		// Green channel
					*pixData++	= col;		// Red channel
					*pixData++;				// Alpha channel
				}
				break;
				case Green:
				{
					col		= *(pixData+1);
					*pixData++	= col;
					*pixData++	= col;
					*pixData++	= col;
					*pixData++;
				}
				break;
					case Blue:
				{
					col		= *pixData;
					*pixData++	= col;
					*pixData++	= col;
					*pixData++	= col;
					*pixData++;
				}
				break;
				case Gray:
				{	// Gray = 0.3*red + 0.59*green + 0.11*blue
					col		= (int)(0.3 * (*(pixData+2)) + 0.59 * (*(pixData+1)) + 0.11 * (*pixData));
					*pixData++	= col;
					*pixData++	= col;
					*pixData++	= col;
					*pixData++;
				}
				break;
				case AllThres:
				{	// threshold all band
					col		= (int)(0.3 * (*(pixData+2)) + 0.59 * (*(pixData+1)) + 0.11 * (*pixData));
					col		= lut[col];
					*pixData++	= col;
					*pixData++	= col;
					*pixData++	= col;
					*pixData++;
				}
				break;
				case IndThres:
				{	// threshold individual band
					*pixData++	= lut[(*pixData)];
					*pixData++	= lut[(*pixData)];
					*pixData++	= lut[(*pixData)];
					*pixData++;
				}
			}
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Edge detection: prewitt mask
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief edge detection (prewitt mask)
//! \param[in, out] 	img		address of the image to be process
//! \param[in] 		orig		a copy of the original image for reference only
//! \param[in]		thresLevel	threshold level
void IP::prewittMask(QImage& img, QImage orig, int thresLevel)
{
	// Prewitt operator:
	//	Gx =	|	-1	0	1 	|
	//			|	-1	0	1	|
	//			|	-1	0	1	|
	//
	//	Gy =	|	-1	-1	-1	|
	//			|	0	0	0	|
	//			|	1	1	1	|

	gray(orig);	// convert image to gray first

	int gx, gy, grad, width, height;
	gx = gy = grad = 0;
	width 	= img.width();
	height	= img.height();

	unsigned char *pixData, *refData1, *refData2, *refData3;

	for (int y = 0; y < height; y++)
	{
		pixData			= img.scanLine(y);
		if (y != 0 && y != (height-1))
		{
			refData1	= orig.scanLine(y-1);
			refData2	= orig.scanLine(y);
			refData3	= orig.scanLine(y+1);
		}

		for (int x = 0; x < width; x++)
		{	// border is set to 0
			if (x == 0 || x == (width-1))
				grad 	= 0;
			else if (y == 0 || y == (height-1))
				grad	= 0;
			else
			{
				gx = (*(refData1+4) - *(refData1-4)) + (*(refData2+4) - *(refData2-4)) + (*(refData3+4) - *(refData3-4));
				gy = (*(refData3-4) + *(refData3) + *(refData3+4)) - (*(refData1-4) + *(refData1) + *(refData1+4));

				refData1	+= 4;
				refData2	+= 4;
				refData3	+= 4;

				grad		= (int)sqrt((double)(gx*gx + gy*gy));		// gradient magnitude; grad=sqrt(gx*gx + gy*gy).
				grad		= grad >  thresLevel ? 255 : 0;				// only consider those values that are above threshold level
			}

			*pixData++	= grad;
			*pixData++	= grad;
			*pixData++	= grad;
			*pixData++;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Edge detection: sobel mask
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief edge detection (sobel mask)
//! \param[in, out] 	img		address of the image to be process
//! \param[in]		orig		a copy of the original image for reference only
//! \param[in]		thresLevel	threshold level
void IP::sobelMask(QImage& img, QImage orig, int thresLevel)
{
	// Sobel operator:
	//	Gx =	|	1	0	-1 	|
	//			|	2	0	-2	|
	//			|	1	0	-1	|
	//
	//	Gy =	|	1	2	1	|
	//			|	0	0	0	|
	//			|	-1	-2	-1	|

	gray(orig);

	int gx, gy, grad, width, height;
	gx = gy = grad = 0;
	width 	= img.width();
	height	= img.height();

	unsigned char *pixData, *refData1, *refData2, *refData3;

	for (int y = 0; y < height; y++)
	{
		pixData			= img.scanLine(y);
		if (y != 0 && y != (height-1))
		{
			refData1	= orig.scanLine(y-1);
			refData2	= orig.scanLine(y);
			refData3	= orig.scanLine(y+1);
		}

		for (int x = 0; x < width; x++)
		{
			if (x == 0 || x == (width-1))
				grad 	= 0;
			else if (y == 0 || y == (height-1))
				grad	= 0;
			else
			{
				gx = (*(refData1-4) - *(refData1+4)) + (2 * (*(refData2-4)) - 2 * (*(refData2+4))) + (*(refData3-4) - *(refData3+4));
				gy = (*(refData1-4) + 2 * (*(refData1)) + *(refData1+4)) - (*(refData3-4) + 2 * (*(refData3)) + *(refData3+4));

				refData1	+= 4;
				refData2	+= 4;
				refData3	+= 4;

				grad		= (int)sqrt((double)(gx*gx + gy*gy));
				grad		= grad >  thresLevel ? 255 : 0;
			}

			*pixData++	= grad;
			*pixData++	= grad;
			*pixData++	= grad;
			*pixData++;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Edge detection: laplacian of gaussian mask
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief edge detection (laplacian of gaussian mask)
//! \param[in, out] 	img		address of the image to be process
//! \param[in]		orig		a copy of the original image for reference only
//! \param[in]		thresLevel	threshold level
void IP::LoGMask(QImage& img, QImage orig, int thresLevel)
{
	// LoG operator:
	//	LoG =	|	0	0	1	0	0 	|
	//			|	0	1	2	1	0	|
	//			|	1	2  -16	2	1	|
	//			|	0	1	2	1	0	|
	//			|	0	0	1	0	0 	|

	gray(orig);

	int log, width, height, row1, row2, row3, row4, row5;
	log 	= 0;
	width 	= img.width();
	height	= img.height();

	unsigned char *pixData, *refData1, *refData2, *refData3, *refData4, *refData5;

	for (int y = 0; y < height; y++)
	{
		pixData			= img.scanLine(y);
		if (y > 1 && y < (height-2))
		{
			refData1	= orig.scanLine(y-2);
			refData2	= orig.scanLine(y-1);
			refData3	= orig.scanLine(y);
			refData4	= orig.scanLine(y+1);
			refData5	= orig.scanLine(y+2);
		}

		for (int x = 0; x < width; x++)
		{
			if (x == 0 || x == 1 || x == (width-2) || x == (width-1))
				log 	= 0;
			else if (y == 0 || y == 1 || y == (height-2) || y == (height-1))
				log	= 0;
			else
			{
				row1 = *(refData1);
				row2 = *(refData2-4) + 2 * (*(refData2)) + *(refData2+4);
				row3 = *(refData3-8) + 2 * (*(refData3-4)) - 16 * (*(refData3)) + 2 * (*(refData3+4)) + *(refData3+8);
				row4 = *(refData4-4) + 2 * (*(refData4)) + *(refData4+4);
				row5 = *(refData5);
				log = row1 + row2 + row3 + row4 + row5;

				refData1	+= 4;
				refData2	+= 4;
				refData3	+= 4;
				refData4	+= 4;
				refData5	+= 4;

				if (log > 255)
					log	= 255;
				else if (log < 0)
					log	= 0;

				log		= log >  thresLevel ? 255 : 0;
			}

			*pixData++	= log;
			*pixData++	= log;
			*pixData++	= log;
			*pixData++;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// gray image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief gray image
//! \param[in, out] 	img	address of the image to be convert
void IP::gray(QImage& img)
{	// Gray = 0.3*red + 0.59*green + 0.11*blue
	int col;
	int width 	= img.width();
	int height	= img.height();

	for (int y = 0; y < height; y++)
	{
		unsigned char* pixData	= img.scanLine(y);

		for (int x = 0; x < width; x++)
		{
			col		= (int)(0.3 * (*(pixData+2)) + 0.59 * (*(pixData+1)) + 0.11 * (*pixData));
			*pixData++	= col;
			*pixData++	= col;
			*pixData++	= col;
			*pixData++;
		}
	}
}
