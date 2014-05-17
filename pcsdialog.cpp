// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class PCSDialog
//! \brief 4PCs dialog box class
//!
//! \file pcsdialog.cpp
//! \brief 4PCs dialog box class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "pcsdialog.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constructor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
PCSDialog::PCSDialog(QWidget *p, Qt::WindowFlags f)
	: QWidget(p, f)
{
	m_pid			= 0;
	m_qid			= 1;
	m_delta			= 2.0;
	m_overlap		= 0.4;

	m_4pcsHandler	= new Handler4PCS();

	m_modelInputLay	= new QGridLayout;

	m_dispArea		= new QTextEdit(this);
	m_dispArea		->setReadOnly(true);

	m_labelPid		= new QLabel(tr("Model P id:"), this);
	m_labelQid		= new QLabel(tr("Model Q id:"), this);
	m_labelDelta	= new QLabel(tr("Delta:"), this);
	m_labelOverlap	= new QLabel(tr("Overlap:"), this);

	m_butStart		= new QPushButton(tr("Start"));
	m_butStart		->setEnabled(true);
	m_butApply		= new QPushButton(tr("Apply Transform"));
	m_butApply		->setEnabled(false);
	m_butCancel		= new QPushButton(tr("Cancel"));
	m_butCancel		->setEnabled(true);

	m_spinPid		= new QSpinBox(this);
	m_spinPid		->setRange(0, 24);
	m_spinPid		->setFixedWidth(250);
	m_spinPid		->setValue(m_pid);

	m_spinQid		= new QSpinBox(this);
	m_spinQid		->setRange(0, 24);
	m_spinQid		->setFixedWidth(250);
	m_spinQid		->setValue(m_qid);

	m_spinDelta		= new QDoubleSpinBox(this);
	m_spinDelta		->setRange(0.0, 10.0);
	m_spinDelta		->setFixedWidth(250);
	m_spinDelta		->setValue(m_delta);
	m_spinDelta		->setSingleStep(0.05);

	m_spinOverlap	= new QDoubleSpinBox(this);
	m_spinOverlap	->setRange(0.0, 2.0);
	m_spinOverlap	->setFixedWidth(250);
	m_spinOverlap	->setValue(m_overlap);
	m_spinOverlap	->setSingleStep(0.05);

	QGridLayout *paraLay	= new QGridLayout;
	paraLay			->addWidget(m_labelDelta, 0, 0, Qt::AlignRight);
	paraLay			->addWidget(m_spinDelta, 0, 1);
	paraLay			->addWidget(m_labelOverlap, 1, 0, Qt::AlignRight);
	paraLay			->addWidget(m_spinOverlap, 1, 1);

	QHBoxLayout *hLay		= new QHBoxLayout;
	hLay			->addWidget(m_butStart);
	hLay			->addWidget(m_butApply);
	hLay			->addWidget(m_butCancel);

	m_pcsLay		= new QVBoxLayout(this);
	m_pcsLay		->addLayout(m_modelInputLay);
	m_pcsLay		->addLayout(paraLay);
	m_pcsLay		->addWidget(m_dispArea);
	m_pcsLay		->addLayout(hLay);

	setLayout		(m_pcsLay);

	connect(m_butStart,		SIGNAL(clicked()),				this, SLOT(start()));
	connect(m_butApply,		SIGNAL(clicked()), 				this, SLOT(apply()));
	connect(m_butCancel,	SIGNAL(clicked()), 				this, SIGNAL(cancelled()));
	connect(m_spinPid,		SIGNAL(valueChanged(int)),		this, SLOT(pidChanged(int)));
	connect(m_spinQid,		SIGNAL(valueChanged(int)), 		this, SLOT(qidChanged(int)));
	connect(m_spinDelta,	SIGNAL(valueChanged(double)),	this, SLOT(deltaChanged(double)));
	connect(m_spinOverlap,	SIGNAL(valueChanged(double)),	this, SLOT(overlapChanged(double)));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Setup
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief setup
//! \param[in] f	enum to indicate which type (single or multiple registration)
void PCSDialog::setup(PCS_Funct f)
{
	m_currentFunct	= f;
	clearLay		();

	switch(m_currentFunct)
	{
		case PCS_SINGLE:
			setupSingle();
			m_dispArea		->append(tr("Single registration: Please input parameters above."));
			break;
		case PCS_MULTIPLE:
			setupMultiple();
			m_dispArea		->append(tr("Multiple registrations: Please input parameters above."));
			break;
		default:
			break;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Store point clouds
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief store point clouds
//! \details store the points and run 4PCS; terminate if it returns false
//! \param[in] *p	1st point cloud
//! \param[in] *q	2nd point cloud
//! \param[in,out] success1	indicate whether 1st point cloud was retrieved properly
//! \param[in,out] success2	indicate whether 2nd point cloud was retrieved properly
// store the points and run 4PCS; terminate if it returns false
void PCSDialog::storeClouds(Vertex *p, Vertex *q, bool success1, bool success2)
{
	if (!success1 || !success2)
	{
		m_dispArea	->append(tr("\nPoints clouds were NOT loaded successfully."));
		return;
	}

	m_4pcsHandler	->setParameters((float)m_delta, (float)m_overlap);
	m_4pcsHandler	->storePoints(p, q);
	m_dispArea		->append(tr("\nPoints clouds stored.\nRunning 4PCS."));

	if (m_4pcsHandler->run4PCS())
	{
		m_dispArea	->append(tr("Done."));
		m_butApply	->setEnabled(true);
	}
	else
		m_dispArea	->append(tr("Terminated."));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for start
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for start
void PCSDialog::start()
{
	if (m_pid != m_qid)
		emit retrieveClouds(m_pid, m_qid);
	else
		m_dispArea	->append(tr("Please specify two separate points cloud."));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for apply
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for apply
// this function is still experimental
void PCSDialog::apply()
{
	if (m_4pcsHandler->hasMatrix())
	{
		m_mat = m_4pcsHandler->getMatrix();
		m_dispArea	->append(tr("%1 %2 %3 %4").arg(m_mat[0]).arg(m_mat[1]).arg(m_mat[2]).arg(m_mat[3]));
		m_dispArea	->append(tr("%1 %2 %3 %4").arg(m_mat[4]).arg(m_mat[5]).arg(m_mat[6]).arg(m_mat[7]));
		m_dispArea	->append(tr("%1 %2 %3 %4").arg(m_mat[8]).arg(m_mat[9]).arg(m_mat[10]).arg(m_mat[11]));
		m_dispArea	->append(tr("%1 %2 %3 %4").arg(m_mat[12]).arg(m_mat[13]).arg(m_mat[14]).arg(m_mat[15]));
		emit transform(m_mat, m_pid, m_qid);
//		emit cloud(m_4pcsHandler->getCloud(), m_pid, m_qid);
	}
	else
		m_dispArea	->append(tr("Please run 4PCS first by clicking 'Start'"));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for pid changed
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for pid changed
//! \param[in] id	new id
void PCSDialog::pidChanged(int id)
{
	m_pid			= id;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for qid changed
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for qid changed
//! \param[in] id	new id
void PCSDialog::qidChanged(int id)
{
	m_qid			= id;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for delta changed
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for delta changed
//! \param[in] val	new value
void PCSDialog::deltaChanged(double val)
{
	m_delta			= val;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Slot for overlap changed
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief slot for overlap changed
//! \param[in] val	new value
void PCSDialog::overlapChanged(double val)
{
	m_overlap		= val;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Setup layout for single registration
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief setup layout for single registration
void PCSDialog::setupSingle()
{
	m_modelInputLay	->addWidget(m_labelPid, 0, 0, Qt::AlignRight);
	m_modelInputLay	->addWidget(m_spinPid, 0, 1);
	m_modelInputLay	->addWidget(m_labelQid, 1, 0, Qt::AlignRight);
	m_modelInputLay	->addWidget(m_spinQid, 1, 1);

	m_labelPid		->setVisible(true);
	m_labelQid		->setVisible(true);
	m_spinPid		->setVisible(true);
	m_spinQid		->setVisible(true);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Setup layout for multiple registrations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief setup layout for multiple registrations
// since we have problem with single registration, we didn't proceed to implementing this function
void PCSDialog::setupMultiple()
{

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clear layout
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief clear layout
void PCSDialog::clearLay()
{
	m_dispArea		->clear();
	m_labelPid		->setVisible(false);
	m_labelQid		->setVisible(false);
	m_spinPid		->setVisible(false);
	m_spinQid		->setVisible(false);

	m_butStart		->setEnabled(true);
	m_butApply		->setEnabled(false);
	m_butCancel		->setEnabled(true);
}
