// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class MainWindow
//! \brief Main Window widget class
//!
//! \file mainwindow.h
//! \brief Main Window widget class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef						MAINWINDOW_H
#define 					MAINWINDOW_H
		
#define 					MAX_RECENT_LAY 					5
	
#include					<QtGui/QtGui>
#include					"layoutdialog.h"
#include					"imageInfo.h"
#include					"historyManager.h"
#include					"OpenGLWidget.h"
#include 					"layoutwindow.h"
#include					"ipdialog.h"
#include					"pcsdialog.h"

class MainWindow : public QMainWindow
{
							Q_OBJECT

public:
	//! \brief Constructor
							MainWindow						();

signals:
	//! \brief Signal that tells which recently customized layout was selected.
	void					actionId						(int);

private slots:
	//! \brief Generate layout from the recently customized layout list with the given id.
	void					recentLay						(int);
	//! \brief Open image
	void					open							();
	//! \brief Open depth file
	void					openDepth						();
	//! \brief Open customize dialog box
	void					customize						();
	//! \brief 1x1 layout, pre-fixed.
	void					layout0							();
	//! \brief 1T_1B layout, pre-fixed.
	void					layout1							();
	//! \brief 1T_2B layout, pre-fixed.
	void					layout2							();
	//! \brief 1L_2R layout, pre-fixed.
	void					layout3							();
	//! \brief 2x2 layout, pre-fixed.
	void					layout4							();
	//! \brief receive messages from any class.
	void					message							(QString);
	//! \brief let navigator knows that a new image is created.
	void					imageCreated					(QImage*, QString);
	//! \brief let navigator knows that current frame changed.
	void					frameChanged					(QImage*, QString);
	//! \brief slot for IP color
	void					ipColor							();
	//! \brief slot for IP threshold
	void					ipThreshold						();
	//! \brief slot for IP edge detection
	void					ipEdgeDet						();
	//! \brief slot for when IP dialog is done
	void					ipDone							(int);
	//! \brief slot for registering one pair of point cloud
	void					single4PCS						();
	//! \brief slot for registering multiple pairs of point cloud
	void					multiple4PCS					();
	//! \brief slot for cancelled 4PCS
	void					pcsCancel						();
	//! \brief slot for retrieving point clouds
	void					getClouds						(int, int);
	//! \brief slot for applying transformation retrieved from 4PCS
	void					applyTransform					(double*, int, int);
	//! \brief slot for receiving the cloud and drawing the cloud
	void					drawCloud						(Vertex*, int, int);
	//! \brief slot for dragging from thumbnail view
	void					thumbCell						(int, int);

private:
	//! \brief add recent layout parameter to recent list
	void					addRecentLay					(int, int, int, int*);
	//! \brief update recent layout menu
	void					updateRecentMenu				();
	//! \brief create all actions in the program.
	void					createActions					();
	//! \brief create all menu in the program.
	void					createMenus						();
	//! \brief create all toolbars in the program.
	void					createToolBars					();
	//! \brief create tab widget containing all tabs
	void					createTabWidget					();
	//! \brief creates the group box for the list of thumbnails
	void					createGroupBoxImages			();
	//! \brief create information tab widget
	void					createInformationTabWidget		();
	//! \brief create history tab widget
	void					createImageHistoryTabWidget		();
	//! \brief	create navigator tab
	void					createNavigatorTabWidget		();
	//! \brief create log tab
	void					createLogTabWidget				();
	//! \brief create labels for information tab
	void					setInformationTabWidgetLabels	(imageInfo*);
	//! \brief add thumbnail to the list
	void					addImage						(QString, QImage);

	// Member variables
	QMenu 					*m_menuFile;					// file menu
	QMenu 					*m_menuLayout; 					// layout menu
	QMenu 					*m_recentLayout;				// recent layout menu
	QMenu 					*m_menuIP;						// image processing menu
	QMenu					*m_menu4PCS;					// 4PCS menu

	QAction					*m_actOpen;						// open action
	QAction					*m_actExit;						// exit action
	QAction					*m_actLay0;						// 1x1 layout action
	QAction					*m_actLay1;						// 1T_1B layout action
	QAction					*m_actLay2;						// 1T_2B layout action
	QAction					*m_actLay3;						// 1L_2R layout action
	QAction					*m_actLay4;						// 2x2 layout action
	QAction					*m_customize;					// customize action
	QAction					*m_IPColor;						// color band
	QAction					*m_IPThres;						// thresholding
	QAction					*m_IPEdge;						// edge detection
	QAction					*m_actOpenDepth;				// open depth file
	QAction					*m_act4PCSsingle;				// single registration
	QAction					*m_act4PCSmultiple;				// multiple registration

	LayoutWindow 			*m_lay1;						// layout window on the right
	QDialog					*m_dialog;						// customize dialog box
	QGridLayout				*m_leftLayout;					// layout to hold thumbnail view and tab widgets

	int						m_spec[MAX];					// specification for customize layoutwindow
	int						m_ipTabWidIndex;				// ip tab widget index
	int						m_4pcsTabWidindex;				// 4PCS tab widget index

	QList<QString>			m_recentText;					// a list of recent layout described in text
	QList<int>				m_recentRow;					// a list of recent layout for row
	QList<int>				m_recentCol;					// a list of recent layout for col
	QList<QChar>			m_recentOrient;					// a list of recent layout for orientation
	QList< QList<int> > 	m_recentSpec;					// a list of spec for recent layouts
	QList<QImage>			m_imgDB;						// a list of all images that have been opened

	QSignalMapper 			*m_signalMapper;				// a signal mapper that maps recent layout to the proper parameters

	historyManager			*m_thumbnailManager;			// Stores ImageInfo of all images
	imageInfo 				*m_imageManager;				// Information about an image
	QGroupBox 				*m_groupBoxImages;				// Grouo Bix for displaying icons and names
	QTableWidget 			*m_tableImages;					// Table that holds icon and name of the image

	//Tab widget data
	QTabWidget				*m_tabWidget;					// Tab widget containing navigator, image info, history, log, 4pcs
	QWidget					*m_navigatorTabWidget;			//	Tab widget showing small image and slider
	QWidget					*m_informationTabWidget;		// displays info about image
	QWidget					*m_imageHistoryTabWidget;		// displays history
	QWidget					*m_logTabWidget;				// displays activity log
	IPDialog				*m_ipWidget;					// tab widget to prompt user input for ip
	PCSDialog				*m_pcsWidget;					// tab widget to prompt user input for 4pcs

	//Tab widget grid
	QGridLayout				*m_informationTabGrid;			// Grid layout for information tab
	QGridLayout				*m_logTabGrid;					// grid layout for log tab
	QGridLayout				*m_imageHistoryTabGrid;			// grid layout for history tab

	QTextEdit 				*m_logTabTextEdit;				// stores text in log tab
	QTextEdit 				*m_imageHistoryTabTextEdit;		// stores text in hostory tab
	QString					*m_imageHistoryTabText;			// holds the text for text edit on image history tab
	QString					*m_logTabText;					// stores a line in log tab
	QSlider					*m_SliderNavigator;				// slider for the navigator window
	OpenGLWidget 			*m_OpenGLWidget;				// OpenGL navigator

	//QLabels
	QLabel					*m_imagePathLabel2;				// label for image path on disk
	QLabel					*m_fileSizeLabel2;				// label for size of the image
	QLabel					*m_imageHeightLabel2;			// label for height of the image in pixels
	QLabel					*m_imageWidthLabel2;			// label for width of the image in pixels
	QLabel					*m_dotsPerMeterYLabel2;			// label for the amount of pixels in a physical meter in vertical direction
	QLabel					*m_dotsPerMeterXLabel2;			// label for the amount of pixels in a physical meter in horizontal direction
	QLabel					*m_sizeOfColorTableLabel2;		// the label for the size of the color table for the image
	QLabel					*m_bytesPerScanLineLabel2;		// the label for the number of bytes per image scanline.
	QLabel					*m_numberOfBytesLabel2;			// the label for the number of bytes occupied by the image data.
	QLabel					*m_imageFormatLabel2;			// the label for the format of the image
	QLabel					*m_alphaChannelLabel2;			// the label stating if it has alpha channel
	QLabel					*m_isGrayscaleLabel2;			// the label stating if it is a greyscale image
};
#endif // MAINWINDOW_H
