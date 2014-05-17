// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class historyManager
//! \brief history Manager class
//!
//! \file historyManager.h
//! \brief history Manager class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef		HISTORYMANAGER_H
#define 	HISTORYMANAGER_H

#include 	"imageInfo.h"
#include	<QLinkedList>
#include	<QString>

class historyManager
{
public:
	//! \brief Traverses the list from a given image to the beginning and creates a derivation of the image
	QString prepareHistory(QLinkedList<imageInfo*>::iterator i);
	//! \brief Finds the image in the linked list and returns an iterator.
	QLinkedList<imageInfo*>::iterator findImage(QImage* id);
	//! \brief Adds the imageHistory object to the list of all imageHistory in the thumbnail bar
	void addImageHistory(imageInfo* newImageHistory);

private:
	QLinkedList<imageInfo*>		m_listImagesHistory;		// All images in the thumbnail bar
	QString				m_historyReport;					// Image derivation

};
#endif
