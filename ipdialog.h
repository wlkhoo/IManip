// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class IPDialog
//! \brief IP_Dialog widget class
//!
//! \file ipdialog.h
//! \brief IP_Dialog widget class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef 		IPDIALOG_H
#define 		IPDIALOG_H

#include		<QtGui>
#include		"ip.h"
#include		"OpenGLWidget.h"

class IPDialog : public QWidget
{
			Q_OBJECT

public:
	//! \brief enum for IPDialog; specifying the processing function
	enum		IP_Function		{COLOR, THRESHOLD, EDGE};
	//! \brief Constructor
			IPDialog		(QWidget *p = 0, Qt::WindowFlags f = 0);
	//! \brief set up the dialog box to reflect the appropriate processing function
	void		setup			(IP_Function, QImage);
	//! \brief user is satisfied with the preview, return a processed image with the same parameters
	QImage		retrieveProcImg		();
	//! \brief notify this dialog box that the active image has changed
	void		imageChanged		(QImage);

signals:
	//! \brief notify MainWindow that user has clicked one fo the three confirmation buttons
	void		done			(int);

private slots:
	//! \brief slot for slider changed; change spin box
	void		sliderChanged		(int);
	//! \brief slot for spin box changed; change slider
	void		spinChanged		(int);
	//! \brief slot for display image option changed
	void		imgButToggled		(bool);
	//! \brief slot for IP color options
	void		processColor		();
	//! \brief slot for IP threshold options
	void		processThreshold	();
	//! \brief slot for IP edge detection options
	void		processEdge		();

private:
	//! \brief set up the dialog box with IP color options
	void		setupColor		();
	//! \brief set up the dialog box with IP threshold options
	void		setupThres		();
	//! \brief set up the dialog box with IP edge detection options
	void		setupEdge		();
	//! \brief clear the IP options layout; preparing for a new one
	void		clearOptLay		();

	IP_Function	m_currentFuct;			// which function is currently performing

	IP		*m_ip;						// Image Processing class

	QImage		m_origImg;				// original image
	QImage		m_resultImg;			// result image
	QImage		m_retProcImg;			// return processed image
	OpenGLWidget	*m_ipDisplay;		// ip OpenGL disply

	QGridLayout	*m_optLay;				// layout for various options
	QVBoxLayout	*m_ipLayout;			// layout for ip tab widget

	QGroupBox	*m_boxDisp;				// group box for display
	QGroupBox	*m_boxOpt;				// group box to select options

	QSlider		*m_thresSlider;			// slider for thresholding
	QSpinBox	*m_thresSpin;			// spin box for thresholding

	QSignalMapper	*m_signalMap;		// map pushbutton signals

	//QRadioButton for ip
	QRadioButton	*m_dispOrig;		// radio button for displaying original image
	QRadioButton	*m_dispResult;		// radio button for displaying processed image
	QRadioButton	*m_colorRed;		// radio button to process red channel
	QRadioButton	*m_colorBlue;		// radio button to process blue channel
	QRadioButton	*m_colorGreen;		// radio button to process green channel
	QRadioButton	*m_colorGray;		// radio button to convert to gray
	QRadioButton	*m_thresInd;		// radio button to threshold individual band
	QRadioButton	*m_thresAll;		// radio button to threshold all bands
	QRadioButton	*m_edgePrewitt;		// radio button to perform prewitt edge detection
	QRadioButton	*m_edgeSobel;		// radio button to perform sobel edge detection
	QRadioButton	*m_edgeLoG;			// radio button to perform LoG edge detection

	//QPushButton for ip
	QPushButton	*m_butOk;				// apply the procedure and destroy the widget
	QPushButton	*m_butCancel;			// cancel the operation and destroy the widget
	QPushButton	*m_butApply;			// apply the procedure but do not destroy the widget
};
#endif
