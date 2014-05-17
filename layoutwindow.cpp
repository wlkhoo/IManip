// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class LayoutWindow
//! \brief Layout Window widget class
//!
//! \file layoutwindow.cpp
//! \brief Layout Window implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include		"layoutwindow.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CONSTRUCTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
LayoutWindow::LayoutWindow(QWidget *parent)
			: QSplitter(parent)
{
	m_leaderId				= -1; // leader is null initially.

	// init signal mapper.
	m_signalMapper			= new QSignalMapper(this);
	connect(m_signalMapper,	SIGNAL(mapped(int)),	this, SIGNAL(splitId(int)));
	connect(this,			SIGNAL(splitId(int)),	this, SLOT(splitGridMoved(int)));

	// init clipboard
	m_clipBoard				= new ClipBoard;

	// init splitters and widgets
	for (int i = 0; i < MAX_SPLIT; i++)
	{
		// init frames
		m_wid[i]			= new Frame(i);
		m_wid[i]			->setMinimumSize(100, 100);

		// establish frame connection
		connect(m_wid[i], SIGNAL(frameChangedActive(int)),	this, SLOT(frameActive(int)));
		connect(m_wid[i], SIGNAL(frameChangedNext(int)),	this, SLOT(frameNext(int)));

		// establish edit connection
		connect(m_wid[i], SIGNAL(imgCopy(int)),		this, SLOT(copy(int)));
		connect(m_wid[i], SIGNAL(imgPaste(int)),	this, SLOT(paste(int)));

		// init splitters
		m_hSplit[i]			= new Splitter(Qt::Horizontal);
		m_vSplit[i]			= new Splitter(Qt::Vertical);
		m_gSplit[i]			= new Splitter(Qt::Vertical);

		// establish special vertical splitters connection and mapping
		connect(m_gSplit[i], SIGNAL(handleMoved()),		m_signalMapper, SLOT(map()));
		m_signalMapper		->setMapping(m_gSplit[i], i);
		connect(m_gSplit[i], SIGNAL(splitterMoved(int, int)),	this, SLOT(splitFollow(int, int)));
	}

	reset();

	// initial configuration
	m_idActive				= m_idNext = 0;			// the 1st window is active and next by default (1x1 window)
	m_visWin				= 1; 					// 1 visible window by default
	m_rows					= m_cols = 1;			// 1x1 by default
	m_layout				= GRID;					// grid layout by default
	splitGrid();
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Derive name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief generate a name for a processed image from the original image
//! \details add '~n', where n is a number greater than 1
//! \return		the derived name
// add '~n', where n is a number greater than 1
QString LayoutWindow::deriveName()
{
	QString newName			= m_wid[m_idActive]->name();
	QRegExp rx("*~*.*");

	rx.setPatternSyntax(QRegExp::Wildcard);
	if (rx.exactMatch(newName))
	{	// increment the derivation number
		int index			= newName.indexOf(".");
		char num			= newName.at(index-1).toAscii() + 1;
		newName.replace	(index-1, 1, num);
	}
	else
	{	// first derivation, add ~1 to the image name
		int index			= newName.indexOf(".");
		newName				= newName.insert(index, "~1");
	}
	return newName;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Retrieve Vertex based on ID specified
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief retrieve Vertex based on ID specified
//! \details retrieve vertices from frame id
//! \param[in] id		frame id
//! \param[out] &success	address to store the success value
//! \return		return a struct containing vertices
Vertex* LayoutWindow::getVertexFromID(int id, bool &success)
{
	Vertex* vertices		= m_wid[id]->getVertex(success);
	return vertices;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Apply the transformation matrix to a specified frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief apply the transformation matrix to a specified frame
//! \details retrieve the 2 clouds and apply the transformation matrix to the 2nd
//! \param[in] *mat		pointer to the transformation matrix
//! \param[in] pid		frame 1 id
//! \param[in] qid		frame 2 id
// retrieve the 2 clouds and apply the transformation matrix to the 2nd
void LayoutWindow::applyTrans(double *mat, int pid, int qid)
{
	bool success;
	Vertex *p				= m_wid[pid]->getVertex(success);
	Vertex *q				= m_wid[qid]->getVertex(success);
	m_wid[m_idNext]			->applyTransform(mat, p, q);
	m_idActive				= m_idNext;

	// figure out which is the next frame
	int cnt					= 1;
	m_idNext++;
	m_idNext				%= m_visWin;

	while(m_wid[m_idNext]->isOccupied())
	{
		if(m_idNext >= m_visWin-1)
			m_idNext		= 0;
		else
			m_idNext++;

		cnt++;
		if (cnt >= m_visWin)
		{
			m_idNext		= 0;
			break;
		}
	}
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Render the cloud in a specified frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief render the cloud in a specified frame
//! \details render the cloud
//! \param[in] *q		a struct containig vertices for point cloud
//! \param[in] pid		frame 1 id
//! \param[in] qid		frame 2 id
void LayoutWindow::drawCloud(Vertex *q, int pid, int qid)
{
	bool success;
	Vertex *p			= m_wid[pid]->getVertex(success);
	m_wid[m_idNext]		->drawTransCloud(p, q);
	m_idActive			= m_idNext;

	// figure out which is the next frame
	int cnt				= 1;
	m_idNext++;
	m_idNext			%= m_visWin;

	while(m_wid[m_idNext]->isOccupied())
	{
		if(m_idNext >= m_visWin-1)
			m_idNext	= 0;
		else
			m_idNext++;

		cnt++;
		if (cnt >= m_visWin)
		{
			m_idNext	= 0;
			break;
		}
	}
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Open images
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief open new image
//! \details construct the title bar given the image's base name and display the image
//! \param[in] imgName	the image's base name
//! \param[in] img	the image itself
// construct the title bar given the image's base name and display the image
void LayoutWindow::open(QString imgName, QImage img)
{
	m_wid[m_idNext]		->open(tr("%1 @ 100% %2x%3").arg(imgName).arg(img.width()).arg(img.height()), imgName, img);
	m_idActive			= m_idNext;

	// figure out which is the next frame
	int cnt				= 1;
	m_idNext++;
	m_idNext			%= m_visWin;

	while(m_wid[m_idNext]->isOccupied())
	{
		if(m_idNext >= m_visWin-1)
			m_idNext	= 0;
		else
			m_idNext++;

		cnt++;
		if (cnt >= m_visWin)
		{
			m_idNext	= 0;
			break;
		}
	}
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Open depth file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief open new depth file
//! \details construct the title bar given the file's base name and display the model
//! \param[in] depthName	the file's base name
//! \param[in] filePath		the file path
// construct the title bar given the file's base name and display the model
void LayoutWindow::openDepthImg(QString depthName, string filePath)
{
	m_wid[m_idNext]		->openDepth(tr("%1 @ 100% 128x128").arg(depthName), depthName, filePath);
	m_idActive			= m_idNext;

	// figure out which is the next frame
	int cnt				= 1;
	m_idNext++;
	m_idNext			%= m_visWin;

	while(m_wid[m_idNext]->isOccupied())
	{
		if(m_idNext >= m_visWin-1)
			m_idNext	= 0;
		else
			m_idNext++;

		cnt++;
		if (cnt >= m_visWin)
		{
			m_idNext	= 0;
			break;
		}
	}
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Grid layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Customized grid layout
//! \details set a horizontal splitter to this and special vertical splitters to this horizontal splitter
// set a horizontal splitter to this and special vertical splitters to this horizontal splitter
void LayoutWindow::splitGrid()
{	// since this is a grid, the size of each frame is the same
	int x, y;
	x	= (width()	-	(5*(m_cols-1)))/m_cols;
	y	= (height() -	(5*(m_rows-1)))/m_rows;

	QList<int> size1, size2;

	insertWidget	(0, m_hSplit[0]);									// parent splitter
	m_hSplit[0]				->setGeometry(0, 0, width(), height());		// set parent's geometry
	for (int i = 0 ; i < m_cols; i++)
	{
		size1.append	(x);											// append the size of horizontal splitter (width)
		m_hSplit[0]			->insertWidget(i, m_gSplit[i]);				// insert vertical splitter to parent splitter
		for (int j = 0; j < m_rows; j++)
		{
			size2.append		(y);									// append size for the vertical splitter
			m_gSplit[i]		->insertWidget(j, m_wid[j*m_cols+i]);		// insert a frame into the vertical splitter
			m_wid[j*m_cols+i]	->resize(x, y);							// resize the frame
			m_wid[j*m_cols+i]	->show();								// show the frame
		}
		m_gSplit[i]	->setSizes(size2);									// set the vertical splitter size
		m_gSplit[i]	->show();
		size2.clear	();
	}
	m_hSplit[0]	->setSizes(size1);										// set the horizontal splitter size
	m_hSplit[0]	->show();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Horizontal layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Customized horizontal layout
//! \details similiar to grid, but the inner loop is slightly different
// similiar to grid, but the inner loop is slightly different
void LayoutWindow::splitCustH()
{	// Algorithm is similar to grid, but the height of each frame has to be computed each time
	int x, y, k;

	x	= (width() - (5*(m_cols-1)))/m_cols;
	k	= 0;

	QList<int> size1, size2;

	insertWidget	(0, m_hSplit[0]);
	m_hSplit[0]	->setGeometry(0, 0, width(), height());

	for (int i = 0; i < m_cols; i++)
	{
		size1.append	(x);
		m_hSplit[0]	->insertWidget(i, m_vSplit[i]);
		for (int j = 0; j < m_spec[i]; j++, k++)
		{
			y		= (height() - (5*(m_spec[i]-1)))/m_spec[i];
			size2.append	(y);
			m_vSplit[i]	->insertWidget(j, m_wid[k]);
			m_wid[k]	->resize(x, y);
			m_wid[k]	->show();
		}
		m_vSplit[i]	->setSizes(size2);
		m_vSplit[i]	->show();
		size2.clear	();
	}

	m_hSplit[0]	->setSizes(size1);
	m_hSplit[0]	->show();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Vertical layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Customized vertical layout
//! \details similiar to grid, but the inner loop is slightly different
// similiar to grid, but the inner loop is slightly different
void LayoutWindow::splitCustV()
{	// Algorithm is similar to grid, but the width of each frame has to be computed each time
	int x, y, k;

	y	= (height() - (5*(m_rows-1)))/m_rows;
	k	= 0;

	QList<int> size1, size2;

	insertWidget	(0, m_vSplit[0]);
	m_vSplit[0]	->setGeometry(0, 0, width(), height());

	for (int j = 0; j < m_rows; j++)
	{
		size1.append	(y);
		m_vSplit[0]	->insertWidget(j, m_hSplit[j]);
		for (int i = 0; i < m_spec[j]; i++, k++)
		{
			x		= (width() - (5*(m_spec[j]-1)))/m_spec[j];
			size2.append	(x);
			m_hSplit[j]	->insertWidget(i, m_wid[k]);
			m_wid[k]	->resize(x, y);
			m_wid[k]	->show();
		}
		m_hSplit[j]	->setSizes(size2);
		m_hSplit[j]	->show();
		size2.clear	();
	}

	m_vSplit[0]	->setSizes(size1);
	m_vSplit[0]	->show();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return active frame's image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return active frame's image
//! \details return active frame's image
//! \return	active frame's image
QImage LayoutWindow::activeImage()
{
	return m_wid[m_idActive]	->image();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Customize layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Custom layout, public slot
//! \details setup custom layout dialog
//! \param[in] rows	# of rows
//! \param[in] cols	# of cols
//! \param[in] orientation	orietation of the layout
//! \param[in] spec	a list of specification (how many windows in each row/column)
void LayoutWindow::custLayout(int rows, int cols, int orientation, int *spec)
{
	m_rows	= rows;
	m_cols	= cols;

	reset(); // reset all splitters and widgets
	switch(orientation)
	{
		case GRID:
			m_layout	= GRID;
			m_visWin	= m_rows * m_cols;
			splitGrid	();
		break;
		case HORIZONTAL:
			m_layout	= HORIZONTAL;
			m_visWin	= m_spec[0] = spec[0];
			for (int i = 1; i < m_cols; i++)
			{
				m_spec[i]	= spec[i];	// copy spec list into internal spec list; resolve pointer issues
				m_visWin	+= m_spec[i];	// compute # of visible windows
			}
			splitCustH	();
		break;
		case VERTICAL:
			m_layout	= VERTICAL;
			m_visWin	= m_spec[0] = spec[0];
			for (int i = 1; i < m_rows; i++)
			{
				m_spec[i]	= spec[i];
				m_visWin	+= m_spec[i];
			}
			splitCustV	();
		break;
	}
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Active Frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set active frame
//! \param[in] id frame id
void LayoutWindow::frameActive(int id)
{
	QImage img;

	m_idActive	= id;
	img		= m_wid[m_idActive]->image();

	emit currentFrame(&img, m_wid[m_idActive]->name());
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Next Frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set next frame
//! \param[in] id frame id
void LayoutWindow::frameNext(int id)
{
	m_idNext	= id;
	frameUpdate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Update Frame
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief update all frames
void LayoutWindow::frameUpdate()
{
	// display message to let user know that frames have changed.
	emit changeMsg(tr("Active Frame: %1 Next Frame: %2").arg(m_idActive).arg(m_idNext));
	if (m_idActive == m_idNext)
	{
		// overlapped frames
		m_wid[m_idActive]	->setOverlapFrames();
		for (int i = 0; i < m_visWin; i++)
		{
			if (i != m_idActive)
				m_wid[i]	->setDefaultFrame();
		}
	}
	else
	{
		m_wid[m_idActive]	->setActiveFrame();
		m_wid[m_idNext]		->setNextFrame();
		for (int i = 0; i < m_visWin; i++)
		{
			if ((i != m_idActive) && (i != m_idNext))
				m_wid[i]	->setDefaultFrame();
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Special splitter for grid layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief splitters have been moved, identify the leader; only valid in grid layout
//! \param[in] id splitter id
// splitters have been moved, identify the leader; only valid in grid layout
void LayoutWindow::splitGridMoved(int id)
{
	if(m_leaderId != id)
	{ 	// only change leader when it's different.
		m_leaderId	= id;
		m_splitLeader	= m_gSplit[m_leaderId];
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Have splitters follow leader
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief have all other splitters follow leader; only valid in grid layout
//! \param[in] pos	where to move to.
//! \param[in] index	which splitter to move
// have all other splitters follow leader; only valid in grid layout
void LayoutWindow::splitFollow(int pos, int index)
{
	int count	= m_hSplit[0]-> count();
	for (int i = 0; i < count; i++)
	{
		if (i != m_leaderId)
		{	// when following leader, all other splitters cannot emit signal (to prevent infinite loop)
			m_gSplit[i]	->blockSignals(true);
			m_gSplit[i]	->moveHandle(pos, index);
			m_gSplit[i]	->blockSignals(false);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief handles resizing
//! \details when resizing, just re-generate the current layout
void LayoutWindow::resizeEvent(QResizeEvent *e)
{
	switch(m_layout)
	{
		case GRID:
			splitGrid ();
		break;
		case HORIZONTAL:
			splitCustH ();
		break;
		case VERTICAL:
			splitCustV ();
		break;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Reset
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief reset all splitters and widgets
//! \details orphan all splitters and widgets and hide all widgets
void LayoutWindow::reset()
{
	for (int i = 0; i < MAX_SPLIT; i++)
	{
		m_hSplit[i]	->hide();
		m_vSplit[i]	->hide();
		m_gSplit[i]	->hide();
		m_wid[i]	->hide();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Cut or Copy image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief copy item to clipboard
//! \details copy the image and its title to clipboard
//! \param[in] id frame id
// copy the image and its title to clipboard
void LayoutWindow::copy(int id)
{
	if(m_wid[id]->isOccupied())
	{
		m_clipBoard	->setItem(m_wid[id]->image(), m_wid[id]->name(), m_wid[id]->title());
		emit changeMsg	(tr("Image cut or copy from Frame #%1").arg(id));
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Paste image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief paste item from clipboard
//! \details retrieve image and its title from clipboard
//! \param[in] id frame id
// retrieve image and its title from clipboard
void LayoutWindow::paste(int id)
{
	QString title	= m_clipBoard->clipFrameTitle();
	if (!title.isNull())
	{
		m_wid[id]	->updateTitleBar(title);
		m_wid[id]	->setImage(m_clipBoard->clipImage(), m_clipBoard->clipImageName());
		emit changeMsg	(tr("Image paste to Frame #%1").arg(id));
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ==================================== Splitter class ============================================
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CONSTRUCTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
Splitter::Splitter(Qt::Orientation orient, QWidget *parent)
	: QSplitter(orient, parent)
{
	setHandleWidth(5);
	setPalette(QPalette(Qt::darkGray)); // set splitter to dark gray
	setChildrenCollapsible(false); // all children cannot resize down to 0.
	connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(splitterChanged(int, int)));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Reimplement base class move splitter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief move splitter at index to pos
//! \details directly accessing QSplitter's protected function
//! \param[in] pos	move to pos; for horizontal splitter, this is x-axis; for vertical, this is y-axis
//! \param[in] index	move the splitter at index; index starts at 1
// directly accessing QSplitter's protected function
void Splitter::moveHandle(int pos, int index)
{
	moveSplitter(pos, index);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// signal handle moved
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief splitter has moved
//! \details if the splitter has moved, emit a signal
//! \param[in] pos	the splitter has moved to pos
//! \param[in] index	the splitter at index has been moved
// if the splitter has moved, emit a signal
void Splitter::splitterChanged(int pos, int index)
{
	emit handleMoved();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ================================== ClipBoard class ============================================
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// set item onto clipboard
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set items in clipboard (image and title)
//! \param[in] img	clipped image
//! \param[in] name	clipped image's name
//! \param[in] title	clipped frame's title
void ClipBoard::setItem(QImage img, QString name, QString title)
{
	m_imgClip	= img;
	m_imgName	= name;
	m_frameTitle	= title;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// retrieve image from clipboard
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief retrieve image from clipboard
// retrieve image from clipboard
QImage ClipBoard::clipImage()
{
	return m_imgClip;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// retrieve image name from clipboard
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief retrieve image name from clipboard
// retrieve image name from clipboard
QString ClipBoard::clipImageName()
{
	return m_imgName;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// retrieve frame title from clipboard
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief retrieve frame title from clipboard
// retrieve frame title from clipboard
QString ClipBoard::clipFrameTitle()
{
	return m_frameTitle;
}
