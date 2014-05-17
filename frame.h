// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class Frame
//! \brief Frame class
//!
//! \file frame.h
//! \brief Frame class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef				FRAME_H
#define				FRAME_H

#include			<QtGui>
#include			"OpenGLWidget.h"

// Frame class
class Frame: public QWidget
{
					Q_OBJECT

	public:
	//! \brief Constructor
					Frame				(int id = -1, QWidget * parent = 0);
	//! \brief set frame to be default
	void			setDefaultFrame		();
	//! \brief set frame to be active
	void			setActiveFrame		();
	//! \brief set frame to be next
	void			setNextFrame		();
	//! \brief indicate frames are overlapped
	void			setOverlapFrames	();
	//! \brief open new image
	void			open				(QString, QString, QImage);
	//! \brief open depth file
	void			openDepth			(QString, QString, string);
	//! \brief update title bar
	void			updateTitleBar		(QString);
	//! \brief set the image to be display
	void			setImage			(QImage, QString);
	//! \brief create actions
	void			createActions		();
	//! \brief create menu
	void			createMenu			();
	//! \brief clear image and reset title bar to default
	void			clear				();
	//! \brief frame is active or not
	bool			isActive			();
	//! \brief frame is next or not
	bool			isNext				();
	//! \brief is frame occupied
	bool			isOccupied			();
	//! \brief return image that the frame is holding
	QImage			image				();
	//! \brief return image name
	QString			name				();
	//! \brief return title of the frame
	QString			title				();
	//! \brief retrieve vertex
	Vertex*			getVertex			(bool&);
	//! \brief apply transformation matrix
	void			applyTransform		(double*, Vertex*, Vertex*);
	//! \brief render the transformed cloud
	void			drawTransCloud		(Vertex*, Vertex*);

protected:
	//! \brief filter event
	virtual bool	eventFilter			(QObject*, QEvent*);
	//! \brief drag enter
	void			dragEnterEvent		(QDragEnterEvent*);
	//! \brief drop
	void			dropEvent			(QDropEvent*);

signals:
	//! \brief signal LayoutWindow that this frame became active
	void			frameChangedActive	(int id);
	//! \brief signal LayoutWindow that this frame became next
	void			frameChangedNext	(int id);
	//! \brief copy image from this frame
	void			imgCopy				(int id);
	//! \brief paste image to this frame
	void			imgPaste			(int id);

private slots:
	//! \brief cut this frame image
	void			cut					();
	//! \brief copy this frame image
	void			copy				();
	//! \brief paste image into this frame
	void			paste				();
	//! \brief close image in this frame
	void			close				();
	//! \brief initialize drag
	void			makeDrag			();
	//! \brief update the title bar with zoom factor
	void			setZoom			(float);

private:
	QVBoxLayout		*m_lay;				// Frame layout manager.
	QLabel			*m_barTitle;		// Frame title bar.
	QString			m_title;			// String that holds the frame title.
	QString			m_imgName;			// Image name
	QLabel			*m_barId;			// Frame id bar
	OpenGLWidget	*m_imgWid;			// Display image.
	QImage			m_img;				// Current image.
	int				m_id;				// Frame id.

	bool			m_isActive;			// Active state of frame.
	bool			m_isNext;			// Next state of frame.
	bool			m_isOccupied;		// Whether this frame is holding an image or not.

	QMenu			*m_popUpMenu;		// Pop up menu on top left corner.
	QAction			*m_actCut;			// Cut
	QAction			*m_actCopy;			// Copy
	QAction			*m_actPaste;		// Paste
	QAction			*m_actClose;		// Close image
};
#endif
