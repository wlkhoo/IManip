// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class LayoutDialog
//! \brief Layout Dialog class
//!
//! \file layoutdialog.h
//! \brief Layout Dialog class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef			LAYOUTDIALOG_H
#define			LAYOUTDIALOG_H

#include		<QtGui>
#include		"layoutwindow.h"

#if		MAX_ROWS >= MAX_COLS
	#define		MAX MAX_ROWS
#else
	#define		MAX MAX_COLS
#endif

class LayoutDialog : public QDialog
{
			Q_OBJECT

public:
	//! \brief Constructor
			LayoutDialog		(QDialog *parent = 0);
	//! \brief Retrival function
	int*		retVal			(int&, int&, int&); // Call this function to retrieve layout parameters

private slots:
	//! \brief Ok slot
	void		ok				();
	//! \brief Grid layout selected
	void		toggledGrid		(bool);
	//! \brief Row based layout selected
	void		toggledRow		(bool);
	//! \brief Col based layout selected
	void		toggledCol		(bool);
	//! \brief Customize layout changed
	void		custChanged		(int);

private:
	QLabel		*m_label1;				// Label for # of row
	QLabel		*m_label2;				// Label for # of col
	QLabel		*m_label3;				// Label for # of row/col under customize
	QLabel		*m_optLabel[MAX];		// Label for optionals

	QSpinBox	*m_spin1;				// Spinbox for # of row
	QSpinBox	*m_spin2;				// Spinbox for # of col
	QSpinBox	*m_spin3;				// Spinbox for # of row/col under customize
	QSpinBox	*m_optSpin[MAX];		// Spinbox for optionals

	QGroupBox	*m_boxGrid;				// Grid box
	QGroupBox	*m_boxOpt;				// Optional box

	int		m_val1;						// Holds value for # of row
	int		m_val2;						// Holds value for # of col
	int		m_opt;						// Holds layout orientation; option 0 = grid, option 1 = horizontal, option 2 = vertical
	int		m_layoutSpec[MAX];			// Holds array of values that user input for how many window they want for each row/col

	QRadioButton	*m_buttonGrid;		// Button for grid
	QRadioButton	*m_buttonRow;		// Button for row based
	QRadioButton	*m_buttonCol;		// Button for column based
};
#endif
