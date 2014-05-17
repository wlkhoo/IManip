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
//! \file pcsdialog.h
//! \brief 4PCs dialog box class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef				PCSDIALOG_H
#define 			PCSDIALOG_H

#include			<QtGui>
#include			"Handler4PCS.h"
#include			"openglwidget.h"

class PCSDialog : public QWidget
{
				Q_OBJECT

public:
	//! \brief enum for type of 4PCS registration
	enum			PCS_Funct		{PCS_SINGLE, PCS_MULTIPLE};
	//! \brief Constructor
					PCSDialog		(QWidget *p = 0, Qt::WindowFlags f = 0);
	//! \brief setup
	void			setup			(PCS_Funct);
	//! \brief store point clouds
	void			storeClouds		(Vertex*, Vertex*, bool, bool);

signals:
	//! \brief cancelled signal
	void			cancelled		();
	//! \brief signal to retrieve clouds
	void			retrieveClouds		(int, int);
	//! \brief emit transformation matrix
	void			transform		(double*, int, int);
	//! \brief emit the transformed cloud
	void			cloud			(Vertex*, int, int);

private slots:
	//! \brief slot for start
	void			start			();
	//! \brief slot for apply
	void			apply			();
	//! \brief slot for pid changed
	void			pidChanged		(int);
	//! \brief slot for qid changed
	void			qidChanged		(int);
	//! \brief slot for delta changed
	void			deltaChanged		(double);
	//! \brief slot for overlap changed
	void			overlapChanged		(double);

private:
	//! \brief setup layout for single registration
	void			setupSingle		();
	//! \brief setup layout for multiple registrations
	void			setupMultiple		();
	//! \brief clear layout
	void			clearLay		();

	PCS_Funct		m_currentFunct;			// 4PCS type (single or multiple)

	Handler4PCS		*m_4pcsHandler;			// Handler for 4PCS

	QGridLayout		*m_modelInputLay;		// Dynamic layout for single or multiple
	QVBoxLayout		*m_pcsLay;				// Main layout (top half)

	QTextEdit		*m_dispArea;			// Text area for output messages.

	QLabel			*m_labelPid;			// pid label
	QLabel			*m_labelQid;			// qid label
	QLabel			*m_labelDelta;			// delta value label
	QLabel			*m_labelOverlap;		// overlap value label

	QSpinBox		*m_spinPid;				// pid spinbox
	QSpinBox		*m_spinQid;				// qid spinbox
	QDoubleSpinBox	*m_spinDelta;			// delta value spinbox
	QDoubleSpinBox	*m_spinOverlap;			// overlap value spinbox

	QPushButton		*m_butStart;			// pushbutton for start
	QPushButton		*m_butApply;			// pushbutton for apply
	QPushButton		*m_butCancel;			// pushbutton for cancel

	int				m_pid;					// pid
	int				m_qid;					// qid
	double			m_delta;				// delta
	double			m_overlap;				// overlap
	double			*m_mat;					// transformation matrix
};
#endif
