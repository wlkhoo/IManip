// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date ADecember 11, 2008
//!
//! \class MainWindow
//!
//! \file MainWindow.cpp
//! \brief Mainwindow implementation.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include 			"mainwindow.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CONSTRUCTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
MainWindow::MainWindow()
{
	// init splitters
	QSplitter *mainSplit 	= new QSplitter(this);
	QSplitter *leftSplit 	= new QSplitter(Qt::Vertical, mainSplit);

	// setting up mainwindow
	move 					(0, 0);
	setCentralWidget		(mainSplit);
	setWindowTitle			(tr("Version 2.0 IManip"));

	// init groupBox
	createGroupBoxImages	();

	//initialize the tab widget
	createTabWidget			();

	// init frames layout
	m_lay1					= new LayoutWindow(mainSplit);

	// connect frame signals
	connect(m_lay1, SIGNAL(changeMsg(QString)), 				this, SLOT(message(QString)));
	connect(m_lay1, SIGNAL(newImg(QImage*, QString)),			this, SLOT(imageCreated(QImage*, QString)));
	connect(m_lay1, SIGNAL(currentFrame(QImage*, QString)),		this, SLOT(frameChanged(QImage*, QString)));

	// init actions, menus, and toolbars
	createActions			();
	createMenus				();
	createToolBars			();

	// init layout
	m_leftLayout 			= new QGridLayout(leftSplit);
	leftSplit				->addWidget(m_groupBoxImages);
	leftSplit				->addWidget(m_tabWidget);

	// set mainSplit sizes
	QList<int> 				size1;
	int unit				= width()/4;
	size1.append			(unit);
	size1.append			(width() - unit);
	mainSplit				->setSizes(size1);

	// init dialog
	m_dialog				= new QDialog(this);

	// set leftSplit size
	QList<int>				size2;
	unit					= height()/2;
	size2.append			(unit);
	size2.append			(height() - unit);
	leftSplit				->setSizes(size2);

	// init spec
	for(int i = 0; i < MAX; i++)
		m_spec[i]			= 0;

	// init signal mapper
	m_signalMapper			= new QSignalMapper(this);
	connect(m_signalMapper,	SIGNAL(mapped(int)),	this, SIGNAL(actionId(int)));
	connect(this, 			SIGNAL(actionId(int)),	this, SLOT(recentLay(int)));

	// init history manager
	m_thumbnailManager 	= new historyManager();

	statusBar()			->showMessage(tr("Ready"), 2000);
	m_logTabText		->append(tr("Ready\n"));
	m_logTabTextEdit	->setText((*m_logTabText));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// ========== Below are private functions that create actions, menu, and toolbar =============
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Creating the actions for the menu
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Create all acions in this program.
void MainWindow::createActions()
{
	m_actOpen				= new QAction	(QIcon(":/images/file_open.xpm"), tr("&Open"), this);
	m_actOpen				->setShortcut	(tr("Ctrl+O"));
	m_actOpen				->setStatusTip	(tr("Open new image"));

	m_actExit				= new QAction	(QIcon(":/images/file_quit.xpm"), tr("E&xit"), this);
	m_actExit				->setShortcut	(tr("Ctrl+Q"));
	m_actExit				->setStatusTip	(tr("Exit the program"));

	m_customize				= new QAction	(tr("Customize"), this);
	m_customize				->setStatusTip	(tr("Customize the windows layout"));

	QActionGroup			*layGroup;
	layGroup				= new QActionGroup(this);
	m_actLay0				= new QAction	(QIcon(":/images/win_layout0.xpm"), tr("Layout 1x1"), layGroup);
	m_actLay1				= new QAction	(QIcon(":/images/win_layout1.xpm"), tr("Layout 1T_1B"), layGroup);
	m_actLay2				= new QAction	(QIcon(":/images/win_layout3.xpm"), tr("Layout 1T_2B"), layGroup);
	m_actLay3				= new QAction	(QIcon(":/images/win_layout5.xpm"), tr("Layout 1L_2R"), layGroup);
	m_actLay4				= new QAction	(QIcon(":/images/win_layout7.xpm"), tr("Layout 2x2"), layGroup);
	m_actLay0				->setCheckable	(true);
	m_actLay1				->setCheckable	(true);
	m_actLay2				->setCheckable	(true);
	m_actLay3				->setCheckable	(true);
	m_actLay4				->setCheckable	(true);
	m_actLay0				->setChecked	(true); // 1x1 layout is default

	layGroup				->setExclusive	(true);
	layGroup				->setVisible	(true);

	QActionGroup 			*ipGroup;
	ipGroup					= new QActionGroup(this);
	m_IPColor				= new QAction	(QIcon(":/images/pt_lut.xpm"), tr("Color"), ipGroup);
	m_IPThres				= new QAction	(QIcon(":/images/pt_thr.xpm"), tr("Threshold"), ipGroup);
	m_IPEdge				= new QAction	(QIcon(":/images/nbr_edge.xpm"), tr("Edge detection"), ipGroup);

	ipGroup					->setExclusive	(true);
	ipGroup					->setVisible	(true);

	m_actOpenDepth			= new QAction	(tr("Open Depth file"),			this);
	m_actOpenDepth			->setShortcut	(tr("Ctrl+T"));

	m_act4PCSsingle			= new QAction	(tr("Single registration"),		this);
	m_act4PCSmultiple		= new QAction	(tr("Multiple registrations"),	this);

	connect(m_actOpen,			SIGNAL(triggered()), this, SLOT(open()));
	connect(m_actExit,			SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(m_customize,		SIGNAL(triggered()), this, SLOT(customize()));
	connect(m_actLay0,			SIGNAL(triggered()), this, SLOT(layout0())); // layout 1x1
	connect(m_actLay1,			SIGNAL(triggered()), this, SLOT(layout1())); // layout 1 top, 1 bottom
	connect(m_actLay2,			SIGNAL(triggered()), this, SLOT(layout2())); // layout 1 top, 2 bottom
	connect(m_actLay3,			SIGNAL(triggered()), this, SLOT(layout3())); // layout 1 left, 2 right
	connect(m_actLay4,			SIGNAL(triggered()), this, SLOT(layout4())); // layout 2x2
	connect(m_IPColor,			SIGNAL(triggered()), this, SLOT(ipColor()));
	connect(m_IPThres,			SIGNAL(triggered()), this, SLOT(ipThreshold()));
	connect(m_IPEdge,			SIGNAL(triggered()), this, SLOT(ipEdgeDet()));
	connect(m_actOpenDepth,		SIGNAL(triggered()), this, SLOT(openDepth()));
	connect(m_act4PCSsingle,	SIGNAL(triggered()), this, SLOT(single4PCS()));
	connect(m_act4PCSmultiple,	SIGNAL(triggered()), this, SLOT(multiple4PCS()));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Creates Menu
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Create all menus in this program.
void MainWindow::createMenus()
{
	// File menu
	m_menuFile		= new QMenu	(tr("&File"), this);
	m_menuFile		->addAction	(m_actOpen);
	m_menuFile		->addAction	(m_actOpenDepth);
	m_menuFile		->addSeparator	();
	m_menuFile		->addAction	(m_actExit);

	// Recent layout menu
	m_recentLayout	= new QMenu	(tr("Recently Customized Layout"), this);
	m_recentLayout	->setIcon	(QIcon(":/images/file_recent.xpm"));

	// Predefined layout menu
	m_menuLayout	= new QMenu	(tr("&Layout"), this);
	m_menuLayout	->addMenu	(m_recentLayout);
	m_menuLayout	->addSeparator	();
	m_menuLayout	->addAction	(m_actLay0);
	m_menuLayout	->addAction	(m_actLay1);
	m_menuLayout	->addAction	(m_actLay2);
	m_menuLayout	->addAction	(m_actLay3);
	m_menuLayout	->addAction	(m_actLay4);
	m_menuLayout	->addSeparator	();
	m_menuLayout	->addAction	(m_customize);

	// IP menu
	m_menuIP		= new QMenu	(tr("IP"), this);
	m_menuIP		->addAction	(m_IPColor);
	m_menuIP		->addAction	(m_IPThres);
	m_menuIP		->addAction	(m_IPEdge);

	// 4PCS menu
	m_menu4PCS		= new QMenu	(tr("4PCS"), this);
	m_menu4PCS		->addAction	(m_act4PCSsingle);
	m_menu4PCS		->addAction	(m_act4PCSmultiple);

	menuBar()		->addMenu	(m_menuFile);
	menuBar()		->addMenu	(m_menuLayout);
	menuBar()		->addMenu	(m_menuIP);
	menuBar()		->addMenu	(m_menu4PCS);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Creating toolbars
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Create all toolbars in this program.
void MainWindow::createToolBars()
{
	QToolBar 		*m_toolBarWin;
	m_toolBarWin	= addToolBar	(tr("&Layout"));
	m_toolBarWin	->addAction	(m_actLay0);
	m_toolBarWin	->addAction	(m_actLay1);
	m_toolBarWin	->addAction	(m_actLay2);
	m_toolBarWin	->addAction	(m_actLay3);
	m_toolBarWin	->addAction	(m_actLay4);
	m_toolBarWin	->addSeparator	();
	m_toolBarWin	->addAction	(m_IPColor);
	m_toolBarWin	->addAction	(m_IPThres);
	m_toolBarWin	->addAction	(m_IPEdge);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// ====== Below are private functions that are connected to all the actions =======
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Slot for opening images
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Open slot
//! \details open images
void MainWindow::open()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image File"), QDir::currentPath());
	QFileInfo pathInfo (filePath);

	if(!filePath.isEmpty())
	{
		QImage img (filePath);
		if (img.isNull())
		{
			QMessageBox::information(this, tr("Open"), tr("Cannot open %1.").arg(filePath));
			m_logTabText		->append(tr("Could not open file: "));
			m_logTabText		->append(tr(((pathInfo.fileName()).toAscii())));
			m_logTabText		->append("\n");
			m_logTabTextEdit	->setText((*m_logTabText));
			return;
		}

		// Pass image's name and the image itself to layoutwindow
		m_lay1 				->open(pathInfo.fileName(), img);

		m_imageManager		= new imageInfo(&img, pathInfo, filePath);
		(*m_thumbnailManager).addImageHistory(m_imageManager);
		addImage(pathInfo.fileName(), img);

		// Display image in a preview
		m_OpenGLWidget		->storeImage(pathInfo.fileName(), img);

		setInformationTabWidgetLabels(m_imageManager);

		// record the history
		m_logTabText		->append(tr(((pathInfo.fileName()).toAscii())));
		m_logTabText		->append(tr(" has been opened\n"));
		m_logTabTextEdit	->setText((*m_logTabText));

		statusBar()			->showMessage(tr("%1 has been opened").arg(pathInfo.fileName()), 2000);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Slot for opening depth files
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Open slot
//! \details open depth file
void MainWindow::openDepth()
{
	QString filePath 			= QFileDialog::getOpenFileName(this, tr("Open Depth File (*.ptx)"), QDir::currentPath());
	QFileInfo pathInfo			(filePath);

	if(!filePath.isEmpty())
	{
		if (pathInfo.completeSuffix() != "ptx")
		{
			QMessageBox::information(this, tr("Open Depth file"), tr("%1 is not a depth file (.ptx)").arg(filePath));
			m_logTabText		->append(tr(((pathInfo.fileName()).toAscii())));
			m_logTabText		->append(tr(" is not a depth file (.ptx)\n"));
			m_logTabTextEdit	->setText((*m_logTabText));
			return;
		}

		string temp				= filePath.toStdString();
		m_lay1					->openDepthImg(pathInfo.fileName(), temp);

		m_logTabText			->append(tr(((pathInfo.baseName()).toAscii())));
		m_logTabText			->append(tr(" has been opened\n"));
		m_logTabTextEdit		->setText((*m_logTabText));
		statusBar()				->showMessage(tr("%1 has been opened").arg(pathInfo.baseName()), 2000);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Customize layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Customize slot
//! \details Construct a LayoutDialog box and retrieve the customized layout parameters
// Construct a LayoutDialog box and retrieve the customized layout parameters
void MainWindow::customize()
{
	LayoutDialog *dial	= new LayoutDialog(m_dialog);
	m_lay1			->releaseKeyboard();

	int r, row, col, orient;

	r				= dial->exec(); 			// execute the customized dialog box
	if (r == 1)
	{
		int *spec;
		spec		= dial->retVal(row, col, orient);	// retrieve values from the dialog box
		m_lay1		->custLayout(row, col, orient, spec);	// generate layout based on retrieved values
		addRecentLay	(row, col, orient, spec); 		// update recently customized layout list
		m_actLay0	->setChecked(false);
		m_actLay1	->setChecked(false);
		m_actLay2	->setChecked(false);
		m_actLay3	->setChecked(false);
		m_actLay4	->setChecked(false);
		update		();
	}

	m_lay1			->grabKeyboard();
	delete dial;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Adds recent layout
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief add recent layout parameter to recent list
//! \details construct a recent layout list of size 5; discard old ones
//! \param[in] row	# of rows
//! \param[in] col	# of cols
//! \param[in] orient	orietation of the layout
//! \param[in] spec	a list of specification (how many windows in each row/column)
//construct a recent layout list of size 5; discard old ones
void MainWindow::addRecentLay(int row, int col, int orient, int *spec)
{
	QChar orientation;
	QString specList = "";	// for use as text
	QList<int> tempSpec;	// for use as saved record

	// constructing a string that has a list of parameters that generated a certain layout
	switch (orient)
	{
		// figuring out the layout orientation and its spec
		case LayoutWindow::GRID:
			orientation 	= 'G';
			specList 	= "-1";
			tempSpec.append	(-1);
		break;
		case LayoutWindow::HORIZONTAL:
			orientation	= 'H';
			specList.append	(tr("%1").arg(spec[0]));
			tempSpec.append	(spec[0]);
			for (int i = 1; i < col; i++)
			{
				specList.append(tr(", %1").arg(spec[i]));
				tempSpec.append(spec[i]);
			}
		break;
		case LayoutWindow::VERTICAL:
			orientation	= 'V';
			specList.append	(tr("%1").arg(spec[0]));
			tempSpec.append	(spec[0]);
			for (int i = 1; i < row; i++)
			{
				specList.append(tr(", %1").arg(spec[i]));
				tempSpec.append(spec[i]);
			}
		break;
	}

	QString text = "";
	text.append(tr("Row: [%1] Col: [%2] Orient: [%3] Spec: [%4]").arg(row).arg(col).arg(orientation).arg(specList));

	// remove duplicate
	if (m_recentText.size() >= 1)
	{
		int i	= m_recentText.indexOf(text);
		if (i > -1)
		{
			m_recentText.removeAt(i);
			m_recentRow.removeAt(i);
			m_recentCol.removeAt(i);
			m_recentOrient.removeAt(i);
			m_recentSpec.removeAt(i);
		}
	}

	// prepend the info
	m_recentText.prepend(text);
	m_recentRow.prepend(row);
	m_recentCol.prepend(col);
	m_recentOrient.prepend(orientation);
	m_recentSpec.prepend(tempSpec);

	// if there's more than 5 recent layouts, remove the oldest one.
	while (m_recentText.size() > MAX_RECENT_LAY)
	{
		m_recentText.removeLast();
		m_recentRow.removeLast();
		m_recentCol.removeLast();
		m_recentOrient.removeLast();
		m_recentSpec.removeLast();
	}

	updateRecentMenu	(); // update the menu
	statusBar() 		->showMessage(tr("Layout changed: %1").arg(text), 2000);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Updates recent menu for layouts
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Establish all the proper connection in recent menu
//! \details update (sort and connect) the menu
// update (sort and connect) the menu
void MainWindow::updateRecentMenu()
{
	// clear the recent layout list and put in new ones and establish proper connections
	m_recentLayout		->clear();
	for (int i = 0; i < m_recentText.size(); i++)
	{
		QAction *lay	= new QAction(tr("%1").arg(m_recentText.at(i)), this);
		connect(lay, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
		m_signalMapper	->setMapping(lay, i);
		m_recentLayout	->addAction(lay);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Recent layouts
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Private slot that gets mapped from signalMapper with the given id
//! \details retrieve the specified recent layout parameters and construct it
//! \param[in] id	signal id
// retrieve the specified recent layout parameters and construct it
void MainWindow::recentLay(int id)
{
	int orient;

	if(m_recentOrient.at(id) == 'V')
		orient		= LayoutWindow::VERTICAL;
	else if (m_recentOrient.at(id) == 'H')
		orient		= LayoutWindow::HORIZONTAL;
	else
		orient		= LayoutWindow::GRID;

	int *spec		= new int [MAX];
	QList<int> tempSpec	= m_recentSpec.at(id);

	for (int i = 0; i < MAX; i++)
		spec[i]		= tempSpec[i];

	m_lay1			->custLayout(m_recentRow.at(id), m_recentCol.at(id), orient, spec);

	// Unselected all pre-defined layout
	m_actLay0		->setChecked(false);
	m_actLay1		->setChecked(false);
	m_actLay2		->setChecked(false);
	m_actLay3		->setChecked(false);
	m_actLay4		->setChecked(false);

	update			();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Layout0
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief layout0 slot
//! \details 1x1 layout
// 1x1 layout
void MainWindow::layout0()
{
	m_spec[0]				= -1;
	m_lay1					->custLayout(1, 1, LayoutWindow::GRID, m_spec);
	statusBar()				->showMessage(tr("Layout changed: 1x1"), 2000);

	m_logTabText			->append(tr("Layout changed: 1x1\n"));
	m_logTabTextEdit		->setText((*m_logTabText));

	update					();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Layout1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief layout1 slot
//! \details 1 top, 1 bottom layout
// 1 top, 1 bottom layout
void MainWindow::layout1()
{
	m_spec[0]			= -1;
	m_lay1				->custLayout(2, 1, LayoutWindow::GRID, m_spec);
	statusBar()			->showMessage(tr("Layout changed: 2x1"), 2000);

	m_logTabText		->append(tr("Layout changed: 2x1\n"));
	m_logTabTextEdit	->setText((*m_logTabText));

	update				();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Layout2
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief layout2 slot
//! \details 1 top, 2 bottom layout
// 1 top, 2 bottom layout
void MainWindow::layout2()
{
	m_spec[0]			= 1;
	m_spec[1]			= 2;
	m_lay1				->custLayout(2, 2, LayoutWindow::VERTICAL, m_spec);
	statusBar()			->showMessage(tr("Layout changed: 1 top, 2 bottom"), 2000);

	m_logTabText		->append(tr("Layout changed: 1 top, 2 bottom\n"));
	m_logTabTextEdit	->setText((*m_logTabText));

	update				();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Layout3
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief layout3 slot
//! \details 1 left, 2 right layout
// 1 left, 2 right layout
void MainWindow::layout3()
{
	m_spec[0]			= 1;
	m_spec[1]			= 2;
	m_lay1				->custLayout(2, 2, LayoutWindow::HORIZONTAL, m_spec);
	statusBar()			->showMessage(tr("Layout changed: 1 left, 2 right"), 2000);

	m_logTabText		->append(tr("Layout changed: 1 left, 2 right\n"));
	m_logTabTextEdit	->setText((*m_logTabText));

	update				();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Layout4
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief layout4 slot
//! \details 2x2 layout
// 2x2 layout
void MainWindow::layout4()
{
	m_spec[0]			= -1;
	m_lay1				->custLayout(2, 2, LayoutWindow::GRID, m_spec);
	statusBar()			->showMessage(tr("Layout changed: 2x2"), 2000);

	m_logTabText		->append(tr("Layout changed: 2x2\n"));
	m_logTabTextEdit	->setText((*m_logTabText));

	update				();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Message
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief receive messages from any class
//! \details display message for 2 seconds
//! \param[in] msg	message to display
// display message for 2 seconds
void MainWindow::message(QString msg)
{
	statusBar()			->showMessage(tr("%1").arg(msg), 2000);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// New image created
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief notify navigator that a new image is created.
//! \details pass image pointer to navigator's class
//! \param[in] img	Image pointer of the image.
//! \param[in] name	the image name
// pass image pointer to navigator's class
void MainWindow::imageCreated(QImage *img, QString name)
{
	m_imageManager		= new imageInfo(img, name);	// generate the image info
	(*m_thumbnailManager).addImageHistory(m_imageManager);	// add the info to history table
	addImage(name, *img);

	m_OpenGLWidget		->storeImage(name, *img);	// display the image in navigator

	setInformationTabWidgetLabels(m_imageManager);

	// record history
	m_logTabText		->append(tr(((name).toAscii())));
	m_logTabText		->append(tr(" has been created\n"));
	m_logTabTextEdit	->setText((*m_logTabText));

	statusBar()			->showMessage(tr("%1 has been createed").arg(name), 2000);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Current frame changed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief notify navigator that current frame has changed
//! \details pass image pointer to navigator's class
//! \param[in] img	Image pointer of the current frame.
//! \param[in] name	the image name
// pass image pointer to navigator's class
void MainWindow::frameChanged(QImage *img, QString name)
{
	// Display image in a preview
	if(!(*img).isNull())
		m_OpenGLWidget	->storeImage(name, *img);
	else
		m_OpenGLWidget	->clear();

	if (m_tabWidget->indexOf(m_ipWidget) != -1)
	{
		if((*img).isNull())
			ipDone(2);	// cannot process null image; as if "Cancel" button has been clicked.
		else
			m_ipWidget	->imageChanged(*img);
	}

	// change image info to reflect current image.
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for IP color
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for IP color
//! \details brings up the IP dialog box with color option setup
// brings up the IP dialog box with color option setup
void MainWindow::ipColor()
{	// get a copy of the active image
	QImage temp			= m_lay1->activeImage();
	if (temp.isNull())
	{	// return if there is no image to process
		statusBar()		->showMessage(tr("Error: There is no image to process"), 2000);
		return;
	}

	// if IPDialog is already in tab widget, remove it; start afresh
	if (m_tabWidget		->indexOf(m_ipWidget) != -1)
		m_tabWidget		->removeTab(m_ipTabWidIndex);

	m_lay1				->releaseKeyboard();				// let IPDialog has keyboard focus
	m_ipWidget			->setup(IPDialog::COLOR, temp);			// set up the appropriate dialog box
	m_ipTabWidIndex		= m_tabWidget->addTab(m_ipWidget, tr("IP"));	// add the dialog box to tab widget
	m_tabWidget			->setCurrentIndex(m_ipTabWidIndex);		// make the dialog the current view
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for IP thresholding
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for IP thresholding
//! \details brings up the IP dialog box with threshold option setup
// brings up the IP dialog box with threshold option setup
void MainWindow::ipThreshold()
{	// similar to ipColor()
	QImage temp			= m_lay1->activeImage();
	if (temp.isNull())
	{
		statusBar()		->showMessage(tr("Error: There is no image to process"), 2000);
		return;
	}

	if (m_tabWidget		->indexOf(m_ipWidget) != -1)
		m_tabWidget		->removeTab(m_ipTabWidIndex);

	m_lay1				->releaseKeyboard();
	m_ipWidget			->setup(IPDialog::THRESHOLD, temp);
	m_ipTabWidIndex		= m_tabWidget->addTab(m_ipWidget, tr("IP"));
	m_tabWidget			->setCurrentIndex(m_ipTabWidIndex);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for IP edge detection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for IP edge detection
//! \details brings up the IP dialog box with edge detection option setup
// brings up the IP dialog box with edge detection option setup
void MainWindow::ipEdgeDet()
{	// similar to ipColor()
	QImage temp			= m_lay1->activeImage();
	if (temp.isNull())
	{
		statusBar()		->showMessage(tr("Error: There is no image to process"), 2000);
		return;
	}

	if (m_tabWidget		->indexOf(m_ipWidget) != -1)
		m_tabWidget		->removeTab(m_ipTabWidIndex);

	m_lay1				->releaseKeyboard();
	m_ipWidget			->setup(IPDialog::EDGE, temp);
	m_ipTabWidIndex		= m_tabWidget->addTab(m_ipWidget, tr("IP"));
	m_tabWidget			->setCurrentIndex(m_ipTabWidIndex);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for when IP dialog is done
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for when IP dialog is done
//! \details handles scenario for all 3 buttons (Ok, Apply Transform, and Cancel)
//! \param[in] val	confirmation value
// handles scenario for all 3 buttons (Ok, Apply Transform, and Cancel)
void MainWindow::ipDone(int val)
{	// one of the confirmation buttons in the dialog box has been clicked
	if (val == 1)
	{ // ok button
		m_lay1				->grabKeyboard();					// reinstate keyboard focus to LayoutWindow
		m_tabWidget			->removeTab(m_ipTabWidIndex);		// remove the dialog box from tab widget (not destroyed)
		m_tabWidget			->setCurrentIndex(0);				// change view to 1st widget in the tab

		QImage derivedImg	= m_ipWidget->retrieveProcImg();	// retrieve the processed image
		QString newName		= m_lay1->deriveName();				// always derive name from active frame
		m_lay1 				->open(newName, derivedImg);		// display the processed image
		imageCreated		(&derivedImg, newName);				// notify relevant classes that a new image has been created
	}
	else if (val == 2)
	{ // cancel button
	  // similar to ok button, but without retrieving the processed image
		m_lay1				->grabKeyboard();
		m_tabWidget			->removeTab(m_ipTabWidIndex);
		m_tabWidget			->setCurrentIndex(0);
	}
	else if (val == 3)
	{ // apply button
	  // similar to ok button, but without closing the dialog box; allowing user to make more configurations
		QImage derivedImg	= m_ipWidget->retrieveProcImg();
		QString newName		= m_lay1->deriveName();
		m_lay1 				->open(newName, derivedImg);
		imageCreated		(&derivedImg, newName);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for registering one pair of point cloud
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for registering one pair of point cloud
//! \details brings up 4PCS dialog box with single registration setup
// brings up 4PCS dialog box with single registration setup
void MainWindow::single4PCS()
{
	if (m_tabWidget			->indexOf(m_pcsWidget) != -1)
		m_tabWidget			->removeTab(m_4pcsTabWidindex);

	m_lay1					->releaseKeyboard();
	m_pcsWidget				->setup(PCSDialog::PCS_SINGLE);
	m_4pcsTabWidindex		= m_tabWidget->addTab(m_pcsWidget, tr("4PCS"));
	m_tabWidget				->setCurrentIndex(m_4pcsTabWidindex);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for registering multiple pairs of point cloud
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for registering multiple pairs of point cloud
//! \details brings up 4PCS dialog box with multiple registration setup
// brings up 4PCS dialog box with multiple registration setup
void MainWindow::multiple4PCS()
{
	if (m_tabWidget			->indexOf(m_pcsWidget) != -1)
		m_tabWidget			->removeTab(m_4pcsTabWidindex);

	m_lay1					->releaseKeyboard();
	m_pcsWidget				->setup(PCSDialog::PCS_MULTIPLE);
	m_4pcsTabWidindex		= m_tabWidget->addTab(m_pcsWidget, tr("4PCS"));
	m_tabWidget				->setCurrentIndex(m_4pcsTabWidindex);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for cancelled 4PCS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for cancelled 4PCS
//! \details cancel; remove dialog box from tab widget
// cancel; remove dialog box from tab widget
void MainWindow::pcsCancel()
{
	m_lay1					->grabKeyboard();
	m_tabWidget				->removeTab(m_4pcsTabWidindex);
	m_tabWidget				->setCurrentIndex(0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for retrieving point clouds
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for retrieving point clouds
//! \details retrive clouds from frames and give it to 4PCS
//! \param[in] p	frame 1 id
//! \param[in] q	frame 2 id
// retrive clouds from frames and give it to 4PCS
void MainWindow::getClouds(int p, int q)
{
	bool success1, success2;

	Vertex *modelp			= m_lay1->getVertexFromID(p, success1);
	Vertex *modelq			= m_lay1->getVertexFromID(q, success2);

	m_pcsWidget				->storeClouds(modelp, modelq, success1, success2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for applying transformation retrieved from 4PCS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for applying transformation retrieved from 4PCS
//! \details apply the transformation matrix to specified frames
//! \param[in] *mat	pointer to the transformation matrix
//! \param[in] pid	frame 1 id
//! \param[in] qid	frame 2 id
// apply the transformation matrix to specified frames
void MainWindow::applyTransform(double *mat, int pid, int qid)
{
	m_lay1					->applyTrans(mat, pid, qid);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for receiving the cloud and drawing the cloud
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for receiving the cloud and drawing the cloud
//! \details render the cloud
//! \param[in] *r	a struct containing vertices for the point cloud
//! \param[in] pid	frame 1 id
//! \param[in] qid	frame 2 id
void MainWindow::drawCloud(Vertex* r, int pid, int qid)
{
	m_lay1					->drawCloud(r, pid, qid);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for dragging from thumbnail view
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for dragging from thumbnail view
//! \details click on any row to drag
//! \param[in] row	signal value emited from thumnail view; which row is clicked
//! \param[in] col	signal value emited from thumnail view; which column is clicked
void MainWindow::thumbCell(int row, int col)
{ // click on any row to drag
	QImage dragImg			= m_imgDB[row];
	QTableWidgetItem *item	= m_tableImages->item(row, 1);
	QString dragImgName		= qvariant_cast<QString>((*item).data(Qt::DisplayRole));

	QMimeData *mData		= new QMimeData;
	mData					->setImageData(dragImg);
	mData					->setText(tr("%1 @ 100% %2x%3").arg(dragImgName).arg(dragImg.width()).arg(dragImg.height()));
	mData					->setData("text", dragImgName.toAscii());

	QDrag *drag				= new QDrag(this);
	drag					->setMimeData(mData);
	drag					->setPixmap(QPixmap(":/images/drag_image.xpm"));
	drag					->start();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Creates a qtablewidget on the left part of the window for displaying
//thumbnails
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Creates the list of icons with names
//! \details Initializes the group box and table and sets properties
void MainWindow::createGroupBoxImages()
{
	m_groupBoxImages					= new QGroupBox(tr("Thumbnail View"));
	m_groupBoxImages					->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QStringList labels;
	labels << tr("Image") << tr("Name");

	m_tableImages						= new QTableWidget;
	m_tableImages						->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	m_tableImages						->setSelectionMode(QAbstractItemView::NoSelection);
	m_tableImages						->setColumnCount(2);
	m_tableImages						->setHorizontalHeaderLabels(labels);

	connect(m_tableImages, SIGNAL(cellPressed(int, int)), this, SLOT(thumbCell(int, int)));

	QHeaderView *headerHorizontalView	= new QHeaderView(Qt::Horizontal, m_tableImages);
	m_tableImages						->setHorizontalHeader(headerHorizontalView);

	headerHorizontalView				->setResizeMode(0, QHeaderView::ResizeToContents);
	headerHorizontalView				->setResizeMode(1, QHeaderView::Stretch);

	QVBoxLayout *layout					= new QVBoxLayout;
	layout								->addWidget(m_tableImages);
	m_groupBoxImages					->setLayout(layout);

	m_tableImages						->verticalHeader()->hide();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Adds a thumbnail view to the qtablewidget
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Adds icon to the list
//! \details Adds thumbnail and its name to the table
void MainWindow::addImage(QString name, QImage img)
{
	m_imgDB			<< img;
	QImage thumbnail					= img.scaled(64, 64, Qt::KeepAspectRatio, Qt::FastTransformation );

	int row								= m_tableImages->rowCount();
	m_tableImages						->setRowCount(row + 1);

	QTableWidgetItem *item0				= new QTableWidgetItem;
	(*item0).setData(Qt::DecorationRole, thumbnail);

	QString *imageName					= new QString(name.toAscii());

	QTableWidgetItem *item1				= new QTableWidgetItem;
	(*item1).setData(Qt::DisplayRole, (*imageName));

	//sets the flags that disable editing of image name
	item0								->setFlags(item0->flags() & (~Qt::ItemIsEditable));
	item1								->setFlags(item1->flags() & (~Qt::ItemIsEditable));

	m_tableImages						->setItem(row , 0, item0);
	m_tableImages						->setItem(row , 1, item1);
	m_tableImages						->setRowHeight(row , 64);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Creates the tab widget
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Creates a tab widget with all tabs
//! \details Also connects signals with slots
void MainWindow::createTabWidget()
{
	createNavigatorTabWidget	();
	createInformationTabWidget	();
	createLogTabWidget			();
	createImageHistoryTabWidget	();

	m_tabWidget 					= new QTabWidget();
	m_tabWidget						->addTab(m_navigatorTabWidget , QString(tr("Navigator")) );
	m_tabWidget						->addTab(m_informationTabWidget , QString(tr("Image Info")) );
	//m_tabWidget						->addTab(m_imageHistoryTabWidget , QString(tr("Image History")) );
	m_tabWidget						->addTab(m_logTabWidget , QString(tr("Log")) );

	m_ipWidget						= new IPDialog();
	connect(m_ipWidget, SIGNAL(done(int)), 						this, SLOT(ipDone(int)));

	m_pcsWidget			= new PCSDialog();
	connect(m_pcsWidget,SIGNAL(cancelled()), 					this, SLOT(pcsCancel()));
	connect(m_pcsWidget,SIGNAL(retrieveClouds(int, int)),		this, SLOT(getClouds(int, int)));
	connect(m_pcsWidget,SIGNAL(transform(double*, int, int)),	this, SLOT(applyTransform(double*, int, int)));
	connect(m_pcsWidget,SIGNAL(cloud(Vertex*, int, int)),		this, SLOT(drawCloud(Vertex*, int, int)));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Creates the tab with the image preview
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Creates navigator tab
//! \details	Contains small icon and  a slider for zooming
void MainWindow::createNavigatorTabWidget ()
{
	m_navigatorTabWidget			= new QWidget();
	QGroupBox *groupBoxNavigator	= new QGroupBox(tr("Navigator"));
	m_OpenGLWidget					= new OpenGLWidget();
	m_OpenGLWidget					->setFixedSize(128, 128);

	m_SliderNavigator				= new QSlider(Qt::Horizontal);
	m_SliderNavigator				->setFocusPolicy(Qt::StrongFocus); // for processing keyboard events
	m_SliderNavigator				->setTickPosition(QSlider::TicksBothSides);
	m_SliderNavigator				->setTickInterval(20);
	m_SliderNavigator				->setSingleStep(1);
	m_SliderNavigator				->setFixedWidth(128);
	m_SliderNavigator				->setRange(1,200);
	m_SliderNavigator				->setValue(100);

	QVBoxLayout *navigatorTabGroupBoxLayout	= new QVBoxLayout;
	navigatorTabGroupBoxLayout				->setAlignment(Qt::AlignHCenter);
	navigatorTabGroupBoxLayout				->addWidget(m_OpenGLWidget);
	navigatorTabGroupBoxLayout				->addWidget(m_SliderNavigator);
	groupBoxNavigator						->setLayout(navigatorTabGroupBoxLayout);

	QVBoxLayout *navigatorTabLayout			= new QVBoxLayout;
	navigatorTabLayout						->addWidget(groupBoxNavigator);

	m_navigatorTabWidget					->setLayout(navigatorTabLayout);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Creates the tab with information about the image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief the tab with information about the image
//! \details List of QLabels using grid layout
void MainWindow::createInformationTabWidget ()
{
	m_informationTabWidget				= new QWidget();
	m_informationTabGrid				= new QGridLayout();

	//Left side
	QLabel *imagePathLabel				= new QLabel(tr("Path:"));
	QLabel *fileSizeLabel				= new QLabel(tr("File size\nin bytes:"));
	QLabel *imageHeightLabel			= new QLabel(tr("Height:"));
	QLabel *imageWidthLabel				= new QLabel(tr("Width:"));
	QLabel *dotsPerMeterYLabel			= new QLabel(tr("Pixels per\nmeter Y:"));
	QLabel *dotsPerMeterXLabel			= new QLabel(tr("Pixels per\nmeter X:"));
	QLabel *numberOfColorsLabel			= new QLabel(tr("Size of\ncolor table:"));
	QLabel *bytesPerScanLineLabel		= new QLabel(tr("Bytes per\nscanline:"));
	QLabel *numberOfBytesLabel			= new QLabel(tr("Number of bytes\noccupied by\nimage data:"));
	QLabel *imageFormatLabel			= new QLabel(tr("Image format:"));
	QLabel *alphaChannelLabel			= new QLabel(tr("Has alpha\nchannel:"));
	QLabel *isGrayscaleLabel			= new QLabel(tr("Is grayscale:"));

	m_informationTabGrid				->addWidget(imagePathLabel, 0, 0);
	m_informationTabGrid				->addWidget(fileSizeLabel, 1, 0);
	m_informationTabGrid				->addWidget(imageHeightLabel, 2, 0);
	m_informationTabGrid				->addWidget(imageWidthLabel, 3, 0);
	m_informationTabGrid				->addWidget(dotsPerMeterYLabel, 4, 0);
	m_informationTabGrid				->addWidget(dotsPerMeterXLabel, 5, 0);
	m_informationTabGrid				->addWidget(numberOfColorsLabel, 6, 0);
	m_informationTabGrid				->addWidget(bytesPerScanLineLabel, 7, 0);
	m_informationTabGrid				->addWidget(numberOfBytesLabel, 8, 0);
	m_informationTabGrid				->addWidget(imageFormatLabel, 9, 0);
	m_informationTabGrid				->addWidget(alphaChannelLabel, 10, 0);
	m_informationTabGrid				->addWidget(isGrayscaleLabel, 11, 0);

	//Right side
	m_imagePathLabel2					= new QLabel(tr("Unknown"));
	m_fileSizeLabel2					= new QLabel(tr("Unknown"));
	m_imageHeightLabel2					= new QLabel(tr("Unknown"));
	m_imageWidthLabel2					= new QLabel(tr("Unknown"));
	m_dotsPerMeterYLabel2				= new QLabel(tr("Unknown"));
	m_dotsPerMeterXLabel2				= new QLabel(tr("Unknown"));
	m_sizeOfColorTableLabel2			= new QLabel(tr("Unknown"));
	m_bytesPerScanLineLabel2			= new QLabel(tr("Unknown"));
	m_numberOfBytesLabel2				= new QLabel(tr("Unknown"));
	m_imageFormatLabel2					= new QLabel(tr("Unknown"));
	m_alphaChannelLabel2				= new QLabel(tr("Unknown"));
	m_isGrayscaleLabel2					= new QLabel(tr("Unknown"));

	m_informationTabGrid				->addWidget(m_imagePathLabel2, 0, 1);
	m_informationTabGrid				->addWidget(m_fileSizeLabel2, 1, 1);
	m_informationTabGrid				->addWidget(m_imageHeightLabel2, 2, 1);
	m_informationTabGrid				->addWidget(m_imageWidthLabel2, 3, 1);
	m_informationTabGrid				->addWidget(m_dotsPerMeterYLabel2, 4, 1);
	m_informationTabGrid				->addWidget(m_dotsPerMeterXLabel2, 5, 1);
	m_informationTabGrid				->addWidget(m_sizeOfColorTableLabel2, 6, 1);
	m_informationTabGrid				->addWidget(m_bytesPerScanLineLabel2, 7, 1);
	m_informationTabGrid				->addWidget(m_numberOfBytesLabel2, 8, 1);
	m_informationTabGrid				->addWidget(m_imageFormatLabel2, 9, 1);
	m_informationTabGrid				->addWidget(m_alphaChannelLabel2, 10, 1);
	m_informationTabGrid				->addWidget(m_isGrayscaleLabel2, 11, 1);

	m_informationTabWidget				->setLayout(m_informationTabGrid);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sets the labels for tab with information about image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Creates labels containg information about image
//! \details Converts everything to string and initializes labels
void MainWindow::setInformationTabWidgetLabels (imageInfo *m_imageManager)
{
	quint16						tempUint16;
	std::stringstream			out;
	std::string					tempString;
	quint32						tempUint32;
	quint64						tempUint64;

	QString *filePath			= new QString(m_imageManager->getimagePath());

	// extract 30-character substring to avoid resizing widget
	if(filePath->length() > 30)
		filePath				->replace(0, filePath->length()-30, "...");

	m_imagePathLabel2			->setText((*filePath));

	//Converting height to string
	tempUint16					= m_imageManager->getImageHeight();
	out							<< tempUint16;
	tempString					= out.str();
	m_imageHeightLabel2			->setText(QString(tempString.c_str()));
	out.str("");

	//Converting width to string
	tempUint16					= m_imageManager->getImageWidth();
	out							<< tempUint16;
	tempString					= out.str();
	m_imageWidthLabel2			->setText(QString(tempString.c_str()));
	out.str("");

	//Converting file size to string
	tempUint64					= m_imageManager->getfileSize();
	out							<< tempUint64;
	tempString					= out.str();
	m_fileSizeLabel2			->setText(QString(tempString.c_str()));
	out.str("");

	//Converting dotsPerMeterY to string
	tempUint16					= m_imageManager->getdotsPerMeterY();
	out							<< tempUint16;
	tempString					= out.str();
	m_dotsPerMeterYLabel2		->setText(QString(tempString.c_str()));
	out.str("");

	//Converting dotsPerMeterX to string
	tempUint16					= m_imageManager->getdotsPerMeterX();
	out							<< tempUint16;
	tempString					= out.str();
	m_dotsPerMeterXLabel2		->setText(QString(tempString.c_str()));
	out.str("");

	//Converting number of colors to string
	tempUint16					= m_imageManager->getsizeOfColorTable();
	out							<< tempUint16;
	tempString					= out.str();
	m_sizeOfColorTableLabel2	->setText(QString(tempString.c_str()));
	out.str("");

	//Converting number of bytes per scanline to string
	tempUint16					= m_imageManager->getbytesPerScanLine();
	out							<< tempUint16;
	tempString					= out.str();
	m_bytesPerScanLineLabel2	->setText(QString(tempString.c_str()));
	out.str("");

	//Converting number of bytes to string
	tempUint32					= m_imageManager->getnumberOfBytes();
	out							<< tempUint16;
	tempString					= out.str();
	m_numberOfBytesLabel2		->setText(QString(tempString.c_str()));

	m_imageFormatLabel2			->setText(m_imageManager->getimageFormat());
	m_alphaChannelLabel2		->setText(m_imageManager->getAlphaChannel());
	m_isGrayscaleLabel2			->setText(m_imageManager->getGrayScale());

	m_informationTabWidget		->update();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Creates the tab with the log
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Creates the tab with log
//! \details Simple text edit
void MainWindow::createLogTabWidget ()
{
	m_logTabWidget				= new QWidget();
	m_logTabGrid				= new QGridLayout();
	m_logTabTextEdit			= new QTextEdit();
	m_logTabText				= new QString(); // to store all text in textedit in the future

	m_logTabTextEdit			->setReadOnly(true);

	m_logTabGrid				->addWidget(m_logTabTextEdit);
	m_logTabWidget				->setLayout(m_logTabGrid);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Creates the tab with the image history
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Creates the tab with image history
//! \details Stores a history in a string
void MainWindow::createImageHistoryTabWidget ()
{
	m_imageHistoryTabWidget		= new QWidget();
	m_imageHistoryTabGrid		= new QGridLayout();
	m_imageHistoryTabTextEdit	= new QTextEdit();

	m_imageHistoryTabTextEdit	->setReadOnly(true);

	m_imageHistoryTabGrid		->addWidget(m_imageHistoryTabTextEdit);
	m_imageHistoryTabWidget		->setLayout(m_imageHistoryTabGrid);
}
