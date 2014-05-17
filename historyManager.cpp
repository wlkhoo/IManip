// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class historyManager
//! \brief history Manager implementation
//!
//! \file historyManager.cpp
//! \brief history Manager implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include 	"historyManager.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finds the image in the linked list and returns an iterator
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Finds the image in the linked list and returns an iterator
//! \details Returns an iterator for an image being searched for
//! \param[in] *id
//! \return Returns an iterator for an image being searched for
// Returns an iterator for an image being searched for
QLinkedList<imageInfo*>::iterator historyManager::findImage(QImage* id)
{
	QLinkedList<imageInfo*>::iterator i,j;

	for (i = m_listImagesHistory.begin(); i != m_listImagesHistory.end(); ++i)
	if((**i).getImagePointer() == id)
	{
			j = i;
			i = m_listImagesHistory.end();
	}
	return j;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Returns a string with description of image derivation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Returns a string with description of image derivation
//! \details Traverses the list from a given image to the beginning and creates a derivation of the image
//! \param[in] i
//! \return Returns a string with description of image derivation
// Traverses the list from a given image to the beginning and creates a derivation of the image
QString historyManager::prepareHistory(QLinkedList<imageInfo*>::iterator i)
{
	m_historyReport.append((**i).getImageName());
	m_historyReport.append(" ");

	if((**i).getParents().isEmpty())
			return m_historyReport;

	m_historyReport.append((**i).getOperation());
	m_historyReport.append(" ");
	m_historyReport.append("(");

	for(QLinkedList<imageInfo*>::iterator j = (**i).getParents().begin(); j!=(**i).getParents().end(); j++)
	{
			m_historyReport.append(prepareHistory(j));
	}

	m_historyReport.append(")");

	return m_historyReport;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Adds new imageInfo object to the list
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Adds new imageInfo object to the list
//! \details Adds the imageHistory object to the list of all imageHistory in the thumbnail bar
//! \param[in] *newImageHistory
// Adds the imageHistory object to the list of all imageHistory in the thumbnail bar
void historyManager::addImageHistory (imageInfo* newImageHistory)
{
	m_listImagesHistory.append(newImageHistory);
}
