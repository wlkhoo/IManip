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
//! \file layoutdialog.cpp
//! \brief Layout Dialog implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include	"layoutdialog.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CONSTRUCTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
LayoutDialog::LayoutDialog(QDialog *parent)
	: QDialog (parent)
{
	// init radio buttons
	m_buttonGrid		= new QRadioButton(tr("Grid"),						this);
	m_buttonRow			= new QRadioButton(tr("Row Based (Vertical)"),		this);
	m_buttonCol			= new QRadioButton(tr("Column Based (Horizontal)"), this);

	// create dummy widget to have all exclusive buttons
	QWidget				*dummy1			= new QWidget;
	QButtonGroup		*buttonGroup1	= new QButtonGroup(dummy1);
	buttonGroup1		->addButton(m_buttonGrid);
	buttonGroup1		->addButton(m_buttonRow);
	buttonGroup1		->addButton(m_buttonCol);

	m_buttonGrid		->setChecked(true);

	// init push button
	QPushButton	*okBut				= new QPushButton(tr("Ok"));
	QPushButton	*cancelBut			= new QPushButton(tr("Cancel"));

	QHBoxLayout	*hButBox			= new QHBoxLayout;
	hButBox				->addStretch(1);
	hButBox				->addWidget(okBut);
	hButBox				->addWidget(cancelBut);

	// init labels and spinboxes
	m_label1			= new QLabel(tr("Row: "),		this);
	m_label2			= new QLabel(tr("Col: "),		this);
	m_label3			= new QLabel(tr("# of row: "),	this);
	m_label3			->setDisabled(true);

	m_spin1				= new QSpinBox(this); // spinbox for # of row
	m_spin2				= new QSpinBox(this); // spinbox for # of col
	m_spin3				= new QSpinBox(this); // spinbox for # of row/col under customize

	// Configure spinboxes
	m_spin1				->setRange(1, MAX_ROWS);
	m_spin1				->setSingleStep(1);
	m_spin1				->setFixedWidth(50);
	m_spin2				->setRange(1, MAX_COLS);
	m_spin2				->setSingleStep(1);
	m_spin2				->setFixedWidth(50);
	m_spin3				->setRange(1, MAX_ROWS);
	m_spin3				->setSingleStep(1);
	m_spin3				->setFixedWidth(50);
	m_spin3				->setDisabled(true);

	// init group box for grid
	m_boxGrid			= new QGroupBox(tr("Grid"), this);

	QGridLayout	*gridOpt = new QGridLayout;
	gridOpt				->addWidget(m_buttonGrid, 0, 0);
	gridOpt				->addWidget(m_label1,	0, 1, Qt::AlignRight);
	gridOpt				->addWidget(m_spin1,	0, 2, Qt::AlignLeft);
	gridOpt				->addWidget(m_label2,	0, 3, Qt::AlignRight);
	gridOpt				->addWidget(m_spin2,	0, 4, Qt::AlignLeft);

	m_boxGrid			->setLayout(gridOpt);

	// init group box for all customize values
	m_boxOpt			= new QGroupBox(tr("Non-grid"), this);

	QGridLayout	*boxOpt = new QGridLayout;
	boxOpt				->addWidget(m_buttonRow, 0, 1);
	boxOpt				->addWidget(m_buttonCol, 0, 2);
	boxOpt				->addWidget(m_label3,		1, 1, Qt::AlignRight);
	boxOpt				->addWidget(m_spin3,		1, 2, Qt::AlignLeft);

	// init customize labels, spinboxes, and values
	m_layoutSpec[0]		= 0;
	m_optLabel[0]		= new QLabel(tr("# of windows for row 1: "), this);
	m_optLabel[0]		->setDisabled(true);
	m_optSpin[0]		= new QSpinBox(this);
	m_optSpin[0]		->setRange(1, MAX_ROWS);
	m_optSpin[0]		->setDisabled(true);

	boxOpt				->addWidget(m_optLabel[0], 3, 1);
	boxOpt				->addWidget(m_optSpin[0], 3, 2);

	for (int i = 1; i < MAX; i++)
	{
		m_optLabel[i]	= new QLabel(tr("# of windows for row %1: ").arg(i+1), this);
		m_optLabel[i]	->setDisabled(true);
		boxOpt			->addWidget(m_optLabel[i], i+3, 1);

		m_optSpin[i]	= new QSpinBox(this);
		m_optSpin[i]	->setRange(1, MAX_ROWS);
		m_optSpin[i]	->setDisabled(true);
		boxOpt			->addWidget(m_optSpin[i], i+3, 2);

		m_layoutSpec[i]	= 0; // init spec array to be all 0
	}

	m_boxOpt			->setLayout(boxOpt);

	// init layout for this dialog box
	QGridLayout	*grid	= new QGridLayout(this);
	grid				->addWidget(m_boxGrid,	0, 0, 1, 5);
	grid				->addWidget(m_boxOpt,	1, 0, 1, 5);
	grid				->addLayout(hButBox,	2, 0, 1, 5);

	connect (m_buttonGrid,	SIGNAL(toggled(bool)),		this, SLOT(toggledGrid(bool)) );
	connect (m_buttonRow,	SIGNAL(toggled(bool)),		this, SLOT(toggledRow(bool)) );
	connect (m_buttonCol,	SIGNAL(toggled(bool)),		this, SLOT(toggledCol(bool)) );
	connect (m_spin3,		SIGNAL(valueChanged(int)),	this, SLOT(custChanged(int)) );
	connect (okBut,			SIGNAL(clicked()),			this, SLOT(ok()) );
	connect (cancelBut,		SIGNAL(clicked()),			this, SLOT(close()) );

	setWindowTitle (tr("Customize Layout"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Retrieval function
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Retrieve values function
//! \param[in, out] val1 Holds the value for # of row
//! \param[in, out] val2 Holds the value for # of col
//! \param[in, out] opt Holds the orientation of the layout
//! \return spec Holds array of values that user input for how many window they want for each row/col
int* LayoutDialog::retVal(int &val1, int &val2, int &opt)
{
	val1	= m_val1;
	val2	= m_val2;
	opt		= m_opt;
	return	m_layoutSpec;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// ========================== Below are private functions ===============================
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Grid layout selected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Private slot when grid is checked
//! \details When grid is checked, certain functions need to be disabled
// When grid is checked, certain functions need to be disabled
void LayoutDialog::toggledGrid(bool checked)
{
	if (checked)
	{
		m_label1		->setDisabled(false);
		m_label1		->setDisabled(false);
		m_spin1			->setDisabled(false);
		m_spin2			->setDisabled(false);
		m_buttonRow		->setDisabled(false);
		m_buttonCol		->setDisabled(false);

		m_label3		->setDisabled(true);
		m_spin3			->setDisabled(true);
		m_optLabel[0]	->setDisabled(true);
		m_optSpin[0]	->setDisabled(true);

		for (int i = 0; i < MAX; i++)
		{
			m_optLabel[i]	->setDisabled(true);
			m_optSpin[i]	->setDisabled(true);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Row based layout selected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Private slot when row based is checked
//! \details When row based is checked, certain functions need to be disabled
// When row based is checked, certain functions need to be disabled
void LayoutDialog::toggledRow(bool checked)
{
	if (checked)
	{
		m_label1		->setDisabled(true);
		m_label1		->setDisabled(true);
		m_spin1			->setDisabled(true);
		m_spin2			->setDisabled(true);

		m_buttonRow		->setDisabled(false);
		m_buttonCol		->setDisabled(false);
		m_label3		->setDisabled(false);
		m_spin3			->setDisabled(false);
		m_spin3			->setRange(1, MAX_ROWS);
		m_optLabel[0]	->setDisabled(false);
		m_optSpin[0]	->setDisabled(false);

		m_label3		->setText(tr("# of row: "));
		for (int i = 0; i < MAX; i++)
		{
			m_optLabel[i]	->setText(tr("# of windows for row %1: ").arg(i+1));
			m_optSpin[i]	->setRange(1, MAX_COLS);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Col based layout selected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Private slot when col based is checked
//! \details When col based is checked, certain functions need to be disabled
// When col based is checked, certain functions need to be disabled
void LayoutDialog::toggledCol(bool checked)
{
	if (checked)
	{
		m_label1		->setDisabled(true);
		m_label1		->setDisabled(true);
		m_spin1			->setDisabled(true);
		m_spin2			->setDisabled(true);

		m_buttonRow		->setDisabled(false);
		m_buttonCol		->setDisabled(false);
		m_label3		->setDisabled(false);
		m_spin3			->setDisabled(false);
		m_spin3			->setRange(1, MAX_COLS);
		m_optLabel[0]	->setDisabled(false);
		m_optSpin[0]	->setDisabled(false);

		m_label3		->setText(tr("# of col: "));
		for (int i = 0; i < MAX; i++)
		{
			m_optLabel[i]	->setText(tr("# of windows for col %1: ").arg(i+1));
			m_optSpin[i]	->setRange(1, MAX_ROWS);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Customize layout changed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Private slot when row/col value changes in customize
//! \details When # of row/col changes, certain labels and spinboxes will be disabled
// When # of row/col changes, certain labels and spinboxes will be disabled.
void LayoutDialog::custChanged(int value)
{
	for (int i = 0; i < value; i++)
	{
		m_optLabel[i]	->setDisabled(false);
		m_optSpin[i]	->setDisabled(false);
	}

	for (int i = value ; i < MAX; i++)
	{
		m_optLabel[i]	->setDisabled(true);
		m_optSpin[i]	->setDisabled(true);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ok slot
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief ok slot
//! \details Record all the values needed for LayoutWindow class and signal done when finished
// Record all the values needed for LayoutWindow class and signal done when finished.
void LayoutDialog::ok()
{
	if (m_buttonGrid->isChecked())
	{
		m_val1			= m_spin1->value();		// return the value for # of row
		m_val2			= m_spin2->value();		// return the value for # of col
		m_opt			= LayoutWindow::GRID;	// return layout type
		m_layoutSpec[0]	= -1;					// return array of spec (in this case, it's -1 b/c of grid).
	}
	else if (m_buttonRow->isChecked())
	{
		m_val1	= m_spin3->value();
		m_val2	= MAX_COLS;
		m_opt	= LayoutWindow::VERTICAL;

		for (int i = 0; i < m_val1; i ++)
			m_layoutSpec[i]	= m_optSpin[i]->value();
	}
	else
	{
		m_val1	= MAX_ROWS;
		m_val2	= m_spin3->value();
		m_opt	= LayoutWindow::HORIZONTAL;

		for (int i = 0; i < m_val2; i++)
			m_layoutSpec[i]	= m_optSpin[i]->value();
	}
	done(1);
}
