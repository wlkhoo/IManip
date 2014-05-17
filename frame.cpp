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
//! \file frame.cpp
//! \brief Frame implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include		"frame.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CONSTRUCTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
Frame::Frame(int id, QWidget * parent)
	:QWidget(parent)
{
	// init actions and menu in this class
	createActions	();
	createMenu	();

	// init private variables
	m_id		= id;
	m_isActive	= m_isNext = m_isOccupied = false;

	// init frame layout
	m_lay		= new QVBoxLayout(this);
	m_lay		->setMargin(2);
	m_lay		->setSpacing(0);
	setLayout	(m_lay);

	// init bar id label
	m_barId		= new QLabel(tr("%1:").arg(m_id));
	m_barId		->setAlignment(Qt::AlignRight);
	m_barId		->setFrameStyle(QFrame::Panel | QFrame::Raised);
	m_barId		->setLineWidth(2);
	m_barId		->setPalette(QPalette(Qt::lightGray));
	m_barId		->setAutoFillBackground(true);
	m_barId		->installEventFilter(this);

	// init initial title text
	m_title		= tr("Untitled @ 0% 0x0");

	// init bar title label
	m_barTitle	= new QLabel(m_title);
	m_barTitle	->setAlignment(Qt::AlignLeft);
	m_barTitle	->setFrameStyle(QFrame::Panel | QFrame::Raised);
	m_barTitle	->setLineWidth(2);
	m_barTitle	->setPalette(QPalette(Qt::lightGray));
	m_barTitle	->setAutoFillBackground(true);
	m_barTitle	->installEventFilter(this);

	// assemble bar id and title
	QHBoxLayout *hLay;
	hLay		= new QHBoxLayout;
	hLay		->setMargin(0);
	hLay		->setSpacing(0);
	hLay		->addWidget(m_barId);
	hLay		->addWidget(m_barTitle);
	hLay		->setStretchFactor(m_barTitle, 1);

	m_lay		->addLayout(hLay);

	// set bar font
	QFont font	= m_barTitle->font();
	font.setPointSize(10);
	font.setWeight(QFont::Bold);
	font.setStyleHint(QFont::SansSerif);

	m_barTitle	->setFont(font);
	m_barId		->setFont(font);

	// image display area
	m_imgWid	= new OpenGLWidget(this);
	m_imgWid	->installEventFilter(this);
	connect(m_imgWid, SIGNAL(dragging()), 		this, SLOT(makeDrag()));
	connect(m_imgWid, SIGNAL(zoomFactor(float)),	this, SLOT(setZoom(float)));

	m_lay		->addWidget(m_imgWid);
	m_lay		->setStretchFactor(m_imgWid, 1);

	setAcceptDrops	(true);
	setMouseTracking(true);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Default frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set frame to be default
void Frame::setDefaultFrame()
{
	// default frame is gray
	m_barId		->setPalette(QPalette(Qt::gray));
	m_barTitle	->setPalette(QPalette(Qt::gray));

	m_isActive	= false;
	m_isNext	= false;

	update		();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Active frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set frame to be active
void Frame::setActiveFrame()
{
	// active frame is cyan
	m_barId		->setPalette(QPalette(Qt::cyan));
	m_barTitle	->setPalette(QPalette(Qt::cyan));

	m_isActive	= true;
	m_isNext	= false;

	m_imgWid	->grabKeyboard();

	update		();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Next frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set frame to be next
void Frame::setNextFrame()
{
	// next frame is green
	m_barId		->setPalette(QPalette(Qt::green));
	m_barTitle	->setPalette(QPalette(Qt::green));

	m_isActive	= false;
	m_isNext	= true;

	update		();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Overlapped frames
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief indicate frames are overlapped
void Frame::setOverlapFrames()
{
	m_barId		->setPalette(QPalette(Qt::red));
	m_barTitle	->setPalette(QPalette(Qt::red));

	m_isActive	= true;
	m_isNext	= true;

	m_imgWid	->grabKeyboard();

	update		();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Active frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief frame is active or not
//! \return 	whether the frame is active or not
bool Frame::isActive()
{
	return m_isActive;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Next frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief frame is next or not
//! \return 	whether the frame is next or not
bool Frame::isNext()
{
	return m_isNext;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Frame occupied
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief is frame occupied
//! \return	whether the frame is occupied or not
bool Frame::isOccupied()
{
	return m_isOccupied;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Open image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief open new image
//! \details display the new image and update the title bar
//! \param[in] title	frame's title
//! \param[in] name	image name
//! \param[in] img	image itself
// display the new image and update the title bar
void Frame::open(QString title, QString name, QImage img)
{
	updateTitleBar	(title);
	m_isOccupied	= true;
	m_img		= img;
	m_imgName	= name;
	m_imgWid	->storeImage(name, m_img);

	m_actCut	->setEnabled(true);
	m_actCopy	->setEnabled(true);
	m_actPaste	->setEnabled(true);
	m_actClose	->setEnabled(true);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Open depth file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief open new depth file
//! \details display the new depth model and update the title bar
//! \param[in] title	frame's title
//! \param[in] name	file's name
//! \param[in] path	file's path
// display the new depth model and update the title bar
void Frame::openDepth(QString title, QString name, string path)
{
	updateTitleBar	(title);
	m_isOccupied	= true;
	m_imgName	= name;
	m_img		= QImage();
	m_imgWid	->storeDepth(name, path);

	m_actCut	->setEnabled(false);
	m_actCopy	->setEnabled(false);
	m_actPaste	->setEnabled(false);
	m_actClose	->setEnabled(true);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Update title bar
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief update title bar
//! \param[in] title	title to be display in bar
void Frame::updateTitleBar(QString title)
{
	m_title		= title;
	m_barTitle	->setText(tr("%1").arg(title));
	m_barTitle	->update();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set image to display
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set the image to be display
//! \param[in] img	image to be display
//! \param[in] name	image's name
void Frame::setImage(QImage img, QString name)
{
	m_img		= img;
	m_isOccupied= true;
	m_imgName	= name;
	m_imgWid	->storeImage(name, m_img);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Event filter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief filter event
//! \details filter out all events except MouseButtonRelease and MouseMove event
//! \param[in] obj	pointer to the object that installed this filter
//! \param[in] e	poinster to the event that invoke this function
//! \return 		return true to filter event out; otherwise, return false
// filter out all events except MouseButtonRelease and MouseMove event
bool Frame::eventFilter(QObject *obj, QEvent *e)
{
	QMouseEvent *me;

	switch(e->type())
	{
		case QEvent::MouseButtonRelease:
			me = (QMouseEvent*)e;
			if (m_barId->underMouse())
			{
				emit frameChangedActive(m_id);
				m_popUpMenu->exec(QCursor::pos());
				return false;
			}
			else
			{
				switch(me->button())
				{
					case Qt::LeftButton: // Set active frame
						emit frameChangedActive(m_id);
						return false;
					case Qt::RightButton: // Set next frame
						emit frameChangedNext(m_id);
						return false;
					default:
						return false;
				}
			}
		default:
			return false;
	}
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Drag
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief initialize drag
//! \details drag the image and its title
// drag the image and its title
void Frame::makeDrag()
{
	// drag the image and its title
	if (m_isOccupied)
	{
		QMimeData *mData	= new QMimeData;
		mData			->setImageData(m_img);
		mData			->setText(m_title);
		mData			->setData("text", m_imgName.toAscii());

		QDrag *drag		= new QDrag(this);
		drag			->setMimeData(mData);
		drag			->setPixmap(QPixmap(":/images/drag_image.xpm"));
		drag			->start();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set zoom factor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief update the title bar with zoom factor
// update the title bar with zoom factor
void Frame::setZoom(float scale)
{
	if(m_img.isNull())
		updateTitleBar(tr("%1 @ %2% 0x0").arg(m_imgName).arg((int)(scale*100)));
	else
		updateTitleBar(tr("%1 @ %2% %3x%4").arg(m_imgName).arg((int)(scale*100)).arg(m_img.width()).arg(m_img.height()));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Drag enter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief drag enter
//! \param[in] e	pointer to the drag enter event
void Frame::dragEnterEvent(QDragEnterEvent *e)
{
	if ((e->mimeData()->hasImage()) && (e->source() != this))
	{	// only accept if it's not dragging back into itself and it has image
		e			->acceptProposedAction();

		if (!(e->source()->inherits("QMainWindow")))
		{	// if the source of the drag is from Frame, clear the source frame
			// do nothing if the source is from QMainWindow
			Frame *f	= (Frame*)(e->source());
			f		->clear();
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Drop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief drop
//! \details drop data into the target frame
// drop data into the target frame
void Frame::dropEvent(QDropEvent *e)
{
	QImage imgDrag	= qvariant_cast<QImage>(e->mimeData()->imageData());
	if(!imgDrag.isNull())
	{
		m_img		= imgDrag;
		m_isOccupied	= true;

		QString title	= qvariant_cast<QString>(e->mimeData()->text());
		updateTitleBar	(title);

		QString temp	(qvariant_cast<QByteArray>(e->mimeData()->data("text")));
		m_imgName	= temp;
		m_imgWid	->storeImage(m_imgName, m_img);

		emit frameChangedActive(m_id);

		m_actCut	->setEnabled(true);
		m_actCopy	->setEnabled(true);
		m_actPaste	->setEnabled(true);
		m_actClose	->setEnabled(true);

		e		->acceptProposedAction();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clear
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief clear image and reset title bar to default
// clear image and reset title bar to default
void Frame::clear()
{
	m_title		= tr("Untitled @ 0% 0x0");
	updateTitleBar	(m_title);
	m_isOccupied	= false;
	m_img		= QImage();
	m_imgName	= QString();
	m_imgWid	->clear();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return frame's image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return image that the frame is holding
//! \return	this frame image
QImage Frame::image()
{
	return m_img;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return image's name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return image name
//! \return	this frame image's name
QString Frame::name()
{
	return m_imgName;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return frame's title
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return title of the frame
//! \return		this frame's title
QString Frame::title()
{
	return m_title;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return frame's vertex
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief retrieve vertex
//! \details get Vertex from image widget (opengl)
//! \param[out] success		indicate whether the retrieval is a success or not
//! \return		return a Vertex struct containing vertices
// get Vertex from image widget (opengl)
Vertex* Frame::getVertex(bool &success)
{
	Vertex *vertices	= NULL;
	if(m_imgWid			->isDepthImg())
	{
		success			= true;
		vertices		= m_imgWid->getVertex();
	}
	else
		success			= false;

	return vertices;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Apply transformation matrix
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief apply transformation matrix
//! \details apply given transformation matrix to the point cloud
//! \param[in] *mat	transformation matrix
//! \param[in] *p	point cloud 1
//! \param[in] *q	point cloud 2
// apply given transformation matrix to the point cloud
void Frame::applyTransform(double *mat, Vertex *p, Vertex *q)
{
	updateTitleBar	(tr("Untitled @ 100% 128x128"));
	m_isOccupied	= true;
	m_imgName		= tr("Untitled");
	m_img			= QImage();

	m_actCut		->setEnabled(false);
	m_actCopy		->setEnabled(false);
	m_actPaste		->setEnabled(false);
	m_actClose		->setEnabled(true);

	m_imgWid		->transformation(mat, p, q);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Render the transformed point cloud
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief render the transformed point cloud
//! \param[in] *p	point cloud 1
//! \param[in] *q	point cloud 2
void Frame::drawTransCloud(Vertex *p, Vertex *q)
{
	updateTitleBar	(tr("Untitled @ 100% 128x128"));
	m_isOccupied	= true;
	m_imgName		= tr("Untitled");
	m_img			= QImage();

	m_actCut		->setEnabled(false);
	m_actCopy		->setEnabled(false);
	m_actPaste		->setEnabled(false);
	m_actClose		->setEnabled(true);

	m_imgWid		->drawCloud(p, q);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Creating the actions for the menu
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief create actions
void Frame::createActions()
{
	m_actCut	= new QAction(QIcon(":/images/edit_cut.xpm"), tr("Cut"), this);
	m_actCopy	= new QAction(QIcon(":/images/edit_copy.xpm"), tr("Copy"), this);
	m_actPaste	= new QAction(QIcon(":/images/edit_paste.xpm"), tr("Paste"), this);
	m_actClose	= new QAction(QIcon(":/images/file_close.xpm"), tr("Close"), this);

	m_actCut	->setEnabled(true);
	m_actCopy	->setEnabled(true);
	m_actPaste	->setEnabled(true);
	m_actClose	->setEnabled(true);

	connect(m_actCut,	SIGNAL(triggered()), 		this, SLOT(cut()));
	connect(m_actCopy,	SIGNAL(triggered()), 		this, SLOT(copy()));
	connect(m_actPaste,	SIGNAL(triggered()), 		this, SLOT(paste()));
	connect(m_actClose,	SIGNAL(triggered()), 		this, SLOT(close()));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Creating Menu
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief create menu
void Frame::createMenu()
{
	m_popUpMenu	= new QMenu(this);
	m_popUpMenu	->addAction(m_actCut);
	m_popUpMenu	->addAction(m_actCopy);
	m_popUpMenu	->addAction(m_actPaste);
	m_popUpMenu	->addAction(m_actClose);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Cut
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief cut this frame image
//! \details copy this frame image into clipboard's memory and clear the frame
// copy this frame image into clipboard's memory and clear the frame
void Frame::cut()
{
	if (!m_img.isNull())
	{
		emit imgCopy(m_id);
		clear();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief copy this frame image
//! \details copy this frame image into clipboard's memory
//  copy this frame image into clipboard's memory
void Frame::copy()
{
	if(!m_img.isNull())
		emit imgCopy(m_id);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Paste
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief paste image into this frame
//! \details retrieve image from clipboard and display on this frame
// retrieve image from clipboard and display on this frame
void Frame::paste()
{
	emit imgPaste(m_id);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Close
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief close image in this frame
// close image in this frame
void Frame::close()
{
	clear();
	m_actCut	->setEnabled(true);
	m_actCopy	->setEnabled(true);
	m_actPaste	->setEnabled(true);
	m_actClose	->setEnabled(true);
}
