// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class LayoutWindow
//! \brief Layout Window splitter class
//!
//! \file layoutwindow.h
//! \brief Layout Window splitter class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef 		LAYOUTWINDOW_H
#define 		LAYOUTWINDOW_H

#define 		MAX_ROWS 		4
#define 		MAX_COLS 		4
#define 		MAX_SPLIT 		MAX_ROWS*MAX_COLS

#include		"frame.h"

class 			Splitter;		// customized QSplitter (below)
class 			ClipBoard;		// clipboard (below)

// LayoutWindow class
class LayoutWindow: public QSplitter
{
			Q_OBJECT

public:
	//! \brief Enum for layouts
	enum 		LayoutOrient		{GRID, HORIZONTAL, VERTICAL};
	//! \brief Constructor
				LayoutWindow		(QWidget* parent=0);
	//! \brief open new image
	void		open				(QString, QImage);
	//! \brief open depth image
	void		openDepthImg		(QString, string);
	//! \brief customized function to generate grid layout
	void		splitGrid			();
	//! \brief customized function to generate horizontal (column based) layout
	void		splitCustH			();
	//! \brief customized function to generate vertical (row based) layout
	void		splitCustV			();
	//! \brief return active frame's image
	QImage		activeImage			();
	//! \brief generate a name for a processed image from the original image (active frame)
	QString		deriveName			();
	//! \brief retrieve Vertex based on ID specified
	Vertex*		getVertexFromID		(int, bool&);
	//! \brief apply the transformation matrix to a specified frame
	void		applyTrans			(double*, int, int);
	//! \brief render the cloud in a specified frame
	void		drawCloud			(Vertex*, int, int);

protected:
	//! \brief handles resizing
	void		resizeEvent			(QResizeEvent*);

signals:
	//! \brief identify which split was clicked; only valid in grid layout
	void		splitId				(int);
	//! \brief send message to mainwindow
	void		changeMsg			(QString);
	//! \brief send image pointer of current frame to mainwindow
	void		currentFrame		(QImage*, QString);
	//! \brief new image is created
	void		newImg				(QImage*, QString);

public slots:
	//! \brief received a new order to generate a new layout with the given parameters
	void		custLayout			(int, int, int, int*);

private slots:
	//! \brief set active frame
	void		frameActive			(int);
	//! \brief set next frame
	void		frameNext			(int);
	//! \brief update frames
	void		frameUpdate			();
	//! \brief splitters have been moved, identify the leader; only valid in grid layout
	void		splitGridMoved		(int);
	//! \brief have all other splitters follow leader; only valid in grid layout
	void		splitFollow			(int, int);
	//! \brief copy item to clipboard
	void		copy				(int);
	//! \brief paste item from clipboard
	void		paste				(int);

private:
	//! \brief a function that reset all splitters and widgets
	void		reset				();

	Splitter	*m_splitLeader;			// the splitter leader; only valid in grid layout
	Splitter	*m_hSplit[MAX_SPLIT];	// horizontal splitters
	Splitter	*m_vSplit[MAX_SPLIT];	// vertical splitters
	Splitter	*m_gSplit[MAX_SPLIT];	// special vertical splitters; only used in grid layout
	Frame		*m_wid[MAX_SPLIT];		// frames

	int		m_visWin;					// # of visible windows.
	int		m_rows;						// # of rows
	int		m_cols;						// # of cols
	int		m_spec[MAX_SPLIT];			// spec for the current layout
	int		m_leaderId;					// leader id
	int		m_layout;					// current layout

	int		m_idActive;					// Active frame id.
	int		m_idNext;					// Next frame id.

	QSignalMapper	*m_signalMapper;	// signal mapper for special vertical splitters (m_gSplit)

	ClipBoard	*m_clipBoard;			// Layout window clipboard
};

// Splitter class
class Splitter : public QSplitter
{
			Q_OBJECT

public:
	//! \brief Constructor
			Splitter				(Qt::Orientation orient = Qt::Horizontal, QWidget *parent = 0);
	//! \brief move splitter to the given position
	void		moveHandle			(int, int);

signals:
	//! \brief signal that splitter has been moved
	void		handleMoved			();

private slots:
	//! \brief splitter moved
	void		splitterChanged		(int, int);
};

// clipboard class -- store one item
class ClipBoard
{
public:
	//! \brief set items in clipboard (image and its name and frame's title)
	void		setItem				(QImage, QString, QString);
	//! \brief retrieve image from clipboard
	QImage		clipImage			();
	//! \brief retrieve image name from clipboard
	QString		clipImageName		();
	//! \brief retrieve frame title from clipboard
	QString		clipFrameTitle		();

private:
	QImage		m_imgClip;			// image
	QString		m_imgName;			// image name
	QString		m_frameTitle;		// frame title
};
#endif
