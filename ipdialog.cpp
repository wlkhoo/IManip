// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class IPDialog
//!
//! \file ipdialog.cpp
//! \brief IP_Dialog implementation.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include 			"ipdialog.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CONSTRUCTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
IPDialog::IPDialog(QWidget *p, Qt::WindowFlags f)
	: QWidget(p, f)
{
	// initialize image processing class
	m_ip			= new IP();

	// create display layout
	QGridLayout *dispLay	= new QGridLayout;
	m_ipDisplay		= new OpenGLWidget();
	m_ipDisplay		->setFixedSize(128, 128);
	dispLay			->addWidget(m_ipDisplay, 0, 0, 2, 2, Qt::AlignCenter);

	m_dispOrig		= new QRadioButton(tr("Original"));
	m_dispResult	= new QRadioButton(tr("Result"));
	m_dispResult	->setChecked(true);			// display result by default
	dispLay			->addWidget(m_dispOrig, 0, 2, Qt::AlignCenter);
	dispLay			->addWidget(m_dispResult, 1, 2, Qt::AlignCenter);

	m_boxDisp		= new QGroupBox(tr("Display"), this);
	m_boxDisp		->setLayout(dispLay);

	// initialize all dynamic layout options
	m_thresSlider	= new QSlider(Qt::Horizontal);
	m_thresSlider	->setTickPosition(QSlider::TicksBelow);
	m_thresSlider	->setRange(0, 255);
	m_thresSlider	->setValue(128);
	m_thresSlider	->setTickInterval(10);

	m_thresSpin		= new QSpinBox;
	m_thresSpin		->setRange(0, 255);
	m_thresSpin		->setValue(128);
	m_thresSpin		->setKeyboardTracking(false);

	m_colorRed		= new QRadioButton(tr("Red"));
	m_colorBlue		= new QRadioButton(tr("Blue"));
	m_colorGreen	= new QRadioButton(tr("Green"));
	m_colorGray		= new QRadioButton(tr("Gray"));
	m_thresInd		= new QRadioButton(tr("Individual"));
	m_thresAll		= new QRadioButton(tr("All"));
	m_edgePrewitt	= new QRadioButton(tr("Prewitt"));
	m_edgeSobel		= new QRadioButton(tr("Sobel"));
	m_edgeLoG		= new QRadioButton(tr("LoG"));

	// dynamic layout depends on function selected
	m_optLay		= new QGridLayout;

	// set different layout depending on the options
	m_boxOpt		= new QGroupBox(this);
	m_boxOpt		->setLayout(m_optLay);

	// initialize confirmation buttons
	m_butOk			= new QPushButton(tr("OK"));
	m_butCancel		= new QPushButton(tr("Cancel"));
	m_butApply		= new QPushButton(tr("Apply"));

	// map confirmation buttons signal
	m_signalMap		= new QSignalMapper(this);
	m_signalMap		->setMapping(m_butOk, 1);
	m_signalMap		->setMapping(m_butCancel, 2);
	m_signalMap		->setMapping(m_butApply, 3);

	QHBoxLayout *hLay	= new QHBoxLayout();
	hLay			->addWidget(m_butOk);
	hLay			->addWidget(m_butCancel);
	hLay			->addWidget(m_butApply);

	// set up the entire dialog layout
	m_ipLayout		= new QVBoxLayout(this);
	m_ipLayout		->addWidget(m_boxDisp);
	m_ipLayout		->setStretchFactor(m_boxDisp, 1);
	m_ipLayout		->addWidget(m_boxOpt);
	m_ipLayout		->addLayout(hLay);

	setLayout		(m_ipLayout);

	// set up all connections
	connect(m_thresSlider, 	SIGNAL(valueChanged(int)), 	this, 			SLOT(sliderChanged(int)));
	connect(m_thresSpin,	SIGNAL(valueChanged(int)), 	this, 			SLOT(spinChanged(int)));
	connect(m_dispOrig,		SIGNAL(toggled(bool)),		this, 			SLOT(imgButToggled(bool))); // true = orig; false = result
	connect(m_colorRed,		SIGNAL(released()),			this, 			SLOT(processColor()));
	connect(m_colorGreen,	SIGNAL(released()),			this, 			SLOT(processColor()));
	connect(m_colorBlue,	SIGNAL(released()),			this, 			SLOT(processColor()));
	connect(m_colorGray,	SIGNAL(released()),			this, 			SLOT(processColor()));
	connect(m_thresInd,		SIGNAL(released()),			this, 			SLOT(processThreshold()));
	connect(m_thresAll,		SIGNAL(released()),			this, 			SLOT(processThreshold()));
	connect(m_edgePrewitt,	SIGNAL(released()),			this, 			SLOT(processEdge()));
	connect(m_edgeSobel,	SIGNAL(released()),			this, 			SLOT(processEdge()));
	connect(m_edgeLoG,		SIGNAL(released()),			this, 			SLOT(processEdge()));
	connect(m_butOk,		SIGNAL(clicked()),			m_signalMap, 	SLOT(map()));
	connect(m_butCancel,	SIGNAL(clicked()),			m_signalMap, 	SLOT(map()));
	connect(m_butApply,		SIGNAL(clicked()),			m_signalMap, 	SLOT(map()));
	connect(m_signalMap,	SIGNAL(mapped(int)),		this, 			SIGNAL(done(int)));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set up the dialog box to reflect the appropriate processing function
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set up the dialog box to reflect the appropriate processing function
//! \param[in] f	enum that indicates which operation to perform
//! \param[in] img	target image
void IPDialog::setup(IP_Function f, QImage img)
{
	m_retProcImg		= img;											// original unscaled copy; used when user is satisfied
	m_origImg			= img.scaled(128, 128,  Qt::KeepAspectRatio);	// for display purpose; show original
	m_resultImg			= img.scaled(128, 128,  Qt::KeepAspectRatio);	// for display purpose; show result
	m_currentFuct		= f;											// current processing function

	clearOptLay						();									// clear IP options layout
	m_dispResult		->setChecked(true);

	switch(m_currentFuct)
	{
		case COLOR:
			m_boxOpt->setTitle(tr("Color"));
			setupColor();												// set up layout
			m_ip	->processImg(IP::Red, m_resultImg);					// default is red
			break;
		case THRESHOLD:
			m_boxOpt->setTitle(tr("Threshold"));
			setupThres();
			m_ip	->lookUpTable(128);									// default threshold level is 128
			m_ip	->processImg(IP::IndThres, m_resultImg);			// default is threshold individual band
			break;
		case EDGE:
			m_boxOpt->setTitle(tr("Edge detection"));
			setupEdge();
			m_ip	->prewittMask(m_resultImg, m_origImg, 128);			// default is prewitt mask with threshold level = 128
			break;
		default:
			break;
	}
	m_ipDisplay		->storeImage(tr("Result"), m_resultImg);			// display the result
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// User is satisfied with the preview, return a processed image with the same parameters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief user is satisfied with the preview, return a processed image with the same parameters
//! \return	processed image
QImage IPDialog::retrieveProcImg()
{
	if (m_currentFuct == COLOR)
	{
		if (m_colorRed		->isChecked())			// Red channel
			m_ip		->processImg(IP::Red, m_retProcImg);
		else if (m_colorBlue	->isChecked())		// Blue channel
			m_ip		->processImg(IP::Blue, m_retProcImg);
		else if (m_colorGreen	->isChecked())		// Green channel
			m_ip		->processImg(IP::Green, m_retProcImg);
		else if (m_colorGray	->isChecked())		// Gray
			m_ip		->processImg(IP::Gray, m_retProcImg);
	}
	else if (m_currentFuct == THRESHOLD)
	{
		m_ip					->lookUpTable(m_thresSpin->value()); 	// read value from spin box and do lut

		if (m_thresInd		->isChecked())						// threshold individual channels
			m_ip				->processImg(IP::IndThres, m_retProcImg);
		else if (m_thresAll	->isChecked())						// threshold all channels
			m_ip				->processImg(IP::AllThres, m_retProcImg);
	}
	else if (m_currentFuct == EDGE)
	{
		QImage refImg(m_retProcImg);
		int thresVal			= m_thresSpin->value();

		if (m_edgePrewitt	->isChecked())						// prewitt edge detection
			m_ip				->prewittMask(m_retProcImg, refImg, thresVal);
		else if (m_edgeSobel->isChecked())						// sobel edge detection
			m_ip				->sobelMask(m_retProcImg, refImg, thresVal);
		else if (m_edgeLoG	->isChecked())						// log edge detection
			m_ip				->LoGMask(m_retProcImg, refImg, thresVal);
	}

	return m_retProcImg;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Notify this dialog box that the active image has changed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief notify this dialog box that the active image has changed
//! \param[in] img	new changed image
void IPDialog::imageChanged(QImage img)
{	// update the changed image
	m_retProcImg		= img;
	m_origImg			= img.scaled(128, 128,  Qt::KeepAspectRatio);
	m_resultImg			= img.scaled(128, 128,  Qt::KeepAspectRatio);

	// reprocess with the new image (same parameters)
	switch(m_currentFuct)
	{
		case COLOR:
			processColor();
			break;
		case THRESHOLD:
			processThreshold();
			break;
		case EDGE:
			processEdge();
			break;
		default:
			break;
	}

	// show the appropriate display
	if (m_dispOrig		->isChecked())
		m_ipDisplay		->storeImage(tr("Original"), m_origImg);
	else
		m_ipDisplay		->storeImage(tr("Result"), m_resultImg);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for slider changed; change spin box
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for slider changed; change spin box
//! \param[in] num	new threshold value
void IPDialog::sliderChanged(int num)
{
	m_thresSpin			->setValue(num);	// notify spinbox

	// reprocess image with new threshold value
	if (m_currentFuct == THRESHOLD)
		processThreshold();
	else if (m_currentFuct == EDGE)
		processEdge();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for spin box changed; change slider
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for spin box changed; change slider
//! \param[in] num	new threshold value
void IPDialog::spinChanged(int num)
{
	m_thresSlider	->setValue(num);	// notify slider

	// reprocess image with new threshold value
	if (m_currentFuct == THRESHOLD)
		processThreshold();
	else if (m_currentFuct == EDGE)
		processEdge();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for display image option changed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for display image option changed
//! \param[in] toggle	switch between orignal and result image
void IPDialog::imgButToggled(bool toggle)
{	// switch display between original and result
	if (toggle)
		m_ipDisplay		->storeImage(tr("Original"), m_origImg);
	else
		m_ipDisplay		->storeImage(tr("Result"), m_resultImg);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for IP color options
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for IP color options
void IPDialog::processColor()
{	// one of the color options has been checked; process the appropriate one
	m_resultImg			= m_origImg;			// make a copy of the original and process it

	if (m_colorRed		->isChecked())			// Red channel
		m_ip			->processImg(IP::Red, m_resultImg);
	else if (m_colorBlue	->isChecked())		// Blue channel
		m_ip			->processImg(IP::Blue, m_resultImg);
	else if (m_colorGreen	->isChecked())		// Green channel
		m_ip			->processImg(IP::Green, m_resultImg);
	else if (m_colorGray	->isChecked())		// Gray
		m_ip			->processImg(IP::Gray, m_resultImg);

	m_ipDisplay			->storeImage(tr("Result"), m_resultImg); // display the new image
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for IP threshold options
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for IP threshold options
void IPDialog::processThreshold()
{	// one of the threshold options has been checked; process the appropriate one
	m_resultImg			= m_origImg;		// make a copy of the original and process it

	m_ip				->lookUpTable(m_thresSpin->value());	// read in threshold value

	if (m_thresInd		->isChecked())							// threshold individual channels
		m_ip			->processImg(IP::IndThres, m_resultImg);
	else if (m_thresAll	->isChecked())							// threshold all channels
		m_ip			->processImg(IP::AllThres, m_resultImg);

	m_ipDisplay			->storeImage(tr("Result"), m_resultImg); // display the new image
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for IP edge detection options
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for IP edge detection options
void IPDialog::processEdge()
{	// one of the edge detection options has been checked; process the appropriate one
	m_resultImg				= m_origImg;					// make a copy of the original and process it

	int thresVal			= m_thresSpin->value();			// read in threshold value

	if (m_edgePrewitt		->isChecked())					// prewitt edge detection
		m_ip				->prewittMask(m_resultImg, m_origImg, thresVal);
	else if (m_edgeSobel	->isChecked())					// sobel edge detection
		m_ip				->sobelMask(m_resultImg, m_origImg, thresVal);
	else if (m_edgeLoG		->isChecked())					// log edge detection
		m_ip				->LoGMask(m_resultImg, m_origImg, thresVal);

	m_ipDisplay				->storeImage(tr("Result"), m_resultImg); // display the new image
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set up the dialog box with IP color options
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set up the dialog box with IP color options
void IPDialog::setupColor()
{	// layout the color radio buttons
	m_optLay				->addWidget(m_colorRed, 0, 0);
	m_optLay				->addWidget(m_colorBlue, 0, 1);
	m_optLay				->addWidget(m_colorGreen, 1, 0);
	m_optLay				->addWidget(m_colorGray, 1, 1);

	m_colorRed				->setChecked(true);						// red by default is checked
	m_colorRed				->setVisible(true);
	m_colorBlue				->setVisible(true);
	m_colorGreen			->setVisible(true);
	m_colorGray				->setVisible(true);

	m_boxOpt				->setLayout(m_optLay);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set up the dialog box with IP threshold options
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set up the dialog box with IP threshold options
void IPDialog::setupThres()
{	// default threshold value is 128
	m_thresSlider			->setValue(128);
	m_thresSpin				->setValue(128);

	// layout the threshold radio button
	m_optLay				->addWidget(m_thresInd, 0, 0, 1, 2, Qt::AlignCenter);
	m_optLay				->addWidget(m_thresAll, 0, 2, Qt::AlignCenter);
	m_optLay				->addWidget(m_thresSlider, 1, 0, 1, 2);
	m_optLay				->addWidget(m_thresSpin, 1, 2);

	m_thresInd				->setChecked(true);	// by default, threshold individual band is chcked
	m_thresInd				->setVisible(true);
	m_thresAll				->setVisible(true);
	m_thresSlider			->setVisible(true);
	m_thresSpin				->setVisible(true);

	m_boxOpt				->setLayout(m_optLay);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set up the dialog box with IP edge detection options
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set up the dialog box with IP edge detection options
void IPDialog::setupEdge()
{	// default threshold value is 128
	m_thresSlider			->setValue(128);
	m_thresSpin				->setValue(128);

	// layout the edge detection radio button
	m_optLay				->addWidget(m_edgePrewitt, 0, 0, Qt::AlignCenter);
	m_optLay				->addWidget(m_edgeSobel, 0, 1, Qt::AlignCenter);
	m_optLay				->addWidget(m_edgeLoG, 0, 2, Qt::AlignCenter);
	m_optLay				->addWidget(m_thresSlider, 1, 0, 1, 2);
	m_optLay				->addWidget(m_thresSpin, 1, 2);

	m_edgePrewitt			->setChecked(true);	// by default, prewitt mask is checked
	m_edgePrewitt			->setVisible(true);
	m_edgeSobel				->setVisible(true);
	m_edgeLoG				->setVisible(true);
	m_thresSlider			->setVisible(true);
	m_thresSpin				->setVisible(true);

	m_boxOpt				->setLayout(m_optLay);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clear the IP options layout; preparing for a new one
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief clear the IP options layout; preparing for a new one
void IPDialog::clearOptLay()
{	// hide all radio buttons
	m_thresSlider			->setVisible(false);
	m_thresSpin				->setVisible(false);
	m_colorRed				->setVisible(false);
	m_colorBlue				->setVisible(false);
	m_colorGreen			->setVisible(false);
	m_colorGray				->setVisible(false);
	m_thresInd				->setVisible(false);
	m_thresAll				->setVisible(false);
	m_edgePrewitt			->setVisible(false);
	m_edgeSobel				->setVisible(false);
	m_edgeLoG				->setVisible(false);
}
