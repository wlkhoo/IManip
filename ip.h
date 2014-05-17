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
//! \file ip.h
//! \brief Image Processing class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef         	IP_H
#define         	IP_H

#include			<QImage>
#include			<cmath>

// IP class
class IP
{
public:
	//! \brief enum for IP class
	enum		IP_FUNCT	{Red, Green, Blue, Gray, AllThres, IndThres};
	//! \brief Constructor
				IP		();
	//! \brief look up table for thresholding
	void		lookUpTable	(int);
	//! \brief process image (point thresholding)
	void		processImg	(IP_FUNCT, QImage&);
	//! \brief edge detection (prewitt mask)
	void		prewittMask	(QImage&, QImage, int);
	//! \brief edge detection (sobel mask)
	void		sobelMask	(QImage&, QImage, int);
	//! \brief edge detection (laplacian of gaussian mask)
	void		LoGMask		(QImage&, QImage, int);

private:
	//! \brief gray image
	void		gray		(QImage&);

	int			*lut;		// look up table array.
};
#endif
