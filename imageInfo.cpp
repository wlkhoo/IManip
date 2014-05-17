// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class imageInfo
//! \brief image Info implementation
//!
//! \file imageInfo.cpp
//! \brief image Info implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include	<QLinkedList>
#include	<QImage>

#include 	"imageInfo.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor when new image created
//! \details Initialize all variables when new image is created
//! \param[in] *pointerImage
//! \param[in] imgName
// Initialize all variables when new image is created
imageInfo::imageInfo(QImage* pointerImage, QString imgName)
{
	m_imagePointer	= pointerImage;
	m_imageName		= imgName;
	m_operation		= "New Image";
	m_imagePath		= imgName.append(" : Image is in memory.");
	m_fileSize		= NULL;

	initializeImageInfoData(pointerImage);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor when opening new image
//! \details Initialize all variables when opening new file
//! \param[in] *pointerImage
//! \param[in] pathInfo
//! \param[in] filePath
// Initialize all variables when opening new file
imageInfo::imageInfo(QImage* pointerImage, QFileInfo pathInfo, QString filePath)
{
	m_imagePointer	= pointerImage;
	m_imageName		= pathInfo.fileName();
	m_operation		= "New File";
	m_imagePath		= filePath;
	m_fileSize		= pathInfo.size();

	initializeImageInfoData(pointerImage);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor when using an operation
//! \details Initialize all variables when using some operation
//! \param[in] pathInfo
//! \param[in] *pointerImage
//! \param[in] parentslst
//! \param[in] op
//Initialize all variables when using some operation
imageInfo::imageInfo(QFileInfo pathInfo, QImage* pointerImage, QLinkedList<imageInfo*> parentslst, QString op)
{
	m_imagePointer 	= pointerImage;
	m_imageName		= pathInfo.fileName().append("~2");
	m_operation		= op;
	m_listParents	= parentslst;
	m_fileSize		= pathInfo.size();

	initializeImageInfoData(pointerImage);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Initializes image data
//! \param[in] *pointerImage
// Initializes image data
void imageInfo::initializeImageInfoData(QImage* pointerImage)
{
	m_imageHeight		= pointerImage->height();
	m_imageWidth		= pointerImage->width();
	m_dotsPerMeterY		= pointerImage->dotsPerMeterY();
	m_dotsPerMeterX		= pointerImage->dotsPerMeterX();
	m_sizeOfColorTable	= pointerImage->numColors();
	m_imageDepth		= pointerImage->depth();
	m_bytesPerScanLine	= pointerImage->bytesPerLine();
	m_numberOfBytes		= pointerImage->numBytes();

	enum QImage::Format result = pointerImage->format();
	if (result == QImage::Format_Invalid)
			m_imageFormat = "Format Invalid";
	else if (result == QImage::Format_Mono)
			m_imageFormat = "Mono";
	else if (result == QImage::Format_MonoLSB)
			m_imageFormat = "MonoLSB";
	else if (result == QImage::Format_Indexed8)
			m_imageFormat = "Indexed 8bit";
	else if (result == QImage::Format_RGB32)
			m_imageFormat = "RGB32";
	else if (result == QImage::Format_ARGB32)
			m_imageFormat = "ARGB32";
	else m_imageFormat = "ARGB32 Premultiplied";

	if (pointerImage->hasAlphaChannel())
			m_hasAlphaChannel = "yes";
	else
			m_hasAlphaChannel = "No";

	if (pointerImage->isGrayscale())
			m_isGrayScale = "Yes";
	else
			m_isGrayScale = "No";
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns a linked list of image's parents
//! \return Returns a linked list of image's parents
// Returns a linked list of image's parents
QLinkedList<imageInfo*>imageInfo::getParents()
{
	return m_listParents;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns an operation used in creating current image
//! \return Returns an operation used in creating current image
// Returns an operation used in creating current image
QString imageInfo::getOperation()
{
	return m_operation;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns image's name
//! \return Returns image's name
// Returns image's name
QString imageInfo::getImageName()
{
	return m_imageName;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns pointer to the image
//! \return Returns pointer to the image
// Returns pointer to the image
QImage* imageInfo::getImagePointer()
{
	return m_imagePointer;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the height of the image
//! \return Gets the height of the image
// Gets the height of the image
quint16 imageInfo::getImageHeight()
{
	return m_imageHeight;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the width of the image
//! \return Gets the width of the image
// Gets the width of the image
quint16 imageInfo::getImageWidth()
{
	return m_imageWidth;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the number of dots per meter of the image height
//! \return Gets the number of dots per meter of the image height
// Gets the number of dots per meter of the image height
quint16 imageInfo::getdotsPerMeterY()
{
	return m_dotsPerMeterY;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the number of dots per meter of the image width
//! \return Gets the number of dots per meter of the image width
// Gets the number of dots per meter of the image width
quint16 imageInfo::getdotsPerMeterX()
{
	return m_dotsPerMeterX;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the number of colors of the image
//! \return Gets the number of colors of the image
// Gets the number of colors of the image
quint16 imageInfo::getsizeOfColorTable()
{
	return m_sizeOfColorTable;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the depth of the image
//! \return Gets the depth of the image
// Gets the depth of the image
quint16 imageInfo::getimageDepth()
{
	return m_imageDepth;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the number of bytes per scanline of the image
//! \return Gets the number of bytes per scanline of the image
// Gets the number of bytes per scanline of the image
quint16 imageInfo::getbytesPerScanLine()
{
	return m_bytesPerScanLine;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Gets the size of the image in bytes
//! \return Gets the size of the image in bytes
// Gets the size of the image in bytes
quint32 imageInfo::getnumberOfBytes()
{
	return m_numberOfBytes;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns the image format
//! \return Returns the image format
// Returns the image format
QString imageInfo::getimageFormat()
{
	return m_imageFormat;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns "yes" or "no" depending whether image has alpha channel
//! \return Returns "yes" or "no" depending whether image has alpha channel
// Returns "yes" or "no" depending whether image has alpha channel
QString imageInfo::getAlphaChannel()
{
	return m_hasAlphaChannel;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns "yes" or "no" depending whether image is grayscale
//! \return Returns "yes" or "no" depending whether image is grayscale
// Returns "yes" or "no" depending whether image is grayscale
QString imageInfo::getGrayScale()
{
	return m_isGrayScale;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns path to the image on the hard drive
//! \return Returns path to the image on the hard drive
// Returns path to the image on the hard drive
QString imageInfo::getimagePath()
{
	return m_imagePath;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns the image file size
//! \return Returns the image file size
// Returns the image file size
quint64 imageInfo::getfileSize()
{
	return m_fileSize;
}
