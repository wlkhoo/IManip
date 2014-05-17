// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class imageInfo
//! \brief stores the operation history for given image
//!
//! \file imageInfo.h
//! \brief image Info class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef 	IMAGEINFO_H
#define 	IMAGEINFO_H

#include	<QLinkedList>
#include	<QImage>
#include	<QFileInfo>
#include	<QString>

class imageInfo
{
public:
	//! \brief Constructor
	imageInfo									(QImage* pointerImage, QString imgName);
	//! \brief Constructor
	imageInfo									(QImage* pointerImage, QFileInfo pathInfo, QString filePath);
	//! \brief Constructor
	imageInfo									(QFileInfo pathInfo, QImage* pointerImage, QLinkedList<imageInfo*> listParents, QString operation);
	//! \brief Initializes image information data for the use in constructor.
	void			initializeImageInfoData		(QImage* pointerImage);
	//! \brief Gets the pointer to the image image.
	QImage*			getImagePointer				();
	//! \brief Gets the name of the images used in creating current image.
	QLinkedList<imageInfo*>	getParents			();
	//! \brief Gets the name of the operation used in creating current images.
	QString			getOperation				();
	//! \brief Gets the size of the image file.
	quint64			getFileSize					();
	//! \brief Gets the name of the image.
	QString			getImageName				();
	//! \brief Gets the height of the image.
	quint16			getImageHeight				();
	//! \brief Gets the width of the image.
	quint16			getImageWidth				();
	//! \brief Gets the number of dots per meter of the image height.
	quint16			getdotsPerMeterY			();
	//! \brief Gets the number of dots per meter of the image width.
	quint16			getdotsPerMeterX			();
	//! \brief Gets the size of color table of the image.
	quint16			getsizeOfColorTable			();
	//! \brief Gets the depth of the image.
	quint16			getimageDepth				();
	//! \brief Gets the number of bytes per scanline of the image.
	quint16			getbytesPerScanLine			();
	//! \brief Gets the size of the image data in bytes.
	quint32			getnumberOfBytes			();
	//! \brief Returns the image format.
	QString			getimageFormat				();
	//! \brief Returns "yes" or "no" depending whether image has alpha channel
	QString			getAlphaChannel				();
	//! \brief Returns "yes" or "no" depending whether image is grayscale.
	QString			getGrayScale				();
	//! \brief Returns path to the image on the hard drive
	QString			getimagePath				();
	//! \brief Returns the size of the image file
	quint64			getfileSize					();

private:
	QImage*			m_imagePointer;				// pointer to the image
	QString			m_imageName;				// image name
	QString     	m_operation;				// operation used to create image

	QLinkedList<imageInfo*>		m_listParents;	// names of images used in creating given image

	quint16			m_imageHeight;				// Stores height of the image in pixels
	quint16			m_imageWidth;				// Stores width of the image in pixels
	quint16			m_dotsPerMeterY;			// Number of pixels fitting vertically in a physical meter
	quint16			m_dotsPerMeterX;			// Number of pixels fitting horizontally in a physical meter
	quint16			m_sizeOfColorTable;			// The size of the color table for the image
	quint16			m_imageDepth;				// The number of bits used to encode a single pixel
	quint16			m_bytesPerScanLine;			// The number of bytes per image scanline. 
	quint32			m_numberOfBytes;			// The number of bytes occupied by the image data.
	QString			m_imageFormat;				// The format of the image
	QString			m_hasAlphaChannel;			// True if the image has a format that respects the alpha channel, otherwise returns false
	QString			m_isGrayScale;				// Checks if image is greyscale
	QString			m_imagePath;				// Stores the path of the image on the disk
	quint64			m_fileSize;					// Stores the size of the image file
};
#endif
