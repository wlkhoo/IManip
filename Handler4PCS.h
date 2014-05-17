// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class Handler4PCS
//! \brief 4PCS handler class
//!
//! \file Handler4PCS.h
//! \brief 4PCS handler class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef				HANDLER4PCS
#define				HANDLER4PCS

//#include			<windows.h>
#include			<stdlib.h>
#include			<math.h>

#include			"pcs4.h"
#include			"openglwidget.h"

#define				_sqr(x)			((x)*(x))
using namespace std;

// include triangle header file
extern "C"
{
	#include "triangle.h"
}

class Handler4PCS
{
public:
	//! \brief Constructor
						Handler4PCS			();
	//! \brief set 4pcs parameters
	void				setParameters		(float, float);
	//! \brief store the point clouds
	void				storePoints			(Vertex*, Vertex*);
	//! \brief run 4pcs
	bool				run4PCS				();
	//! \brief return the transformation matrix
	double*				getMatrix			();
	//! \brief indicate whether it has the matrix or not
	bool				hasMatrix			();
	//! \brief return a copy of the transformed cloud
	Vertex*				getCloud			();

private:
	//! \brief triangulate
	void				triang				(QVector<Point3D>&);
	//! \brief compute face normal
	bool				computeFaceNormal	(Point3D*, Point3D*, Point3D*, Point3D*);
	//! \brief compute vector offset
	void				vectorOffset		(Point3D*, Point3D*, Point3D*);
	//! \brief compute vector normal
	void				vectorGetNormal		(Point3D*, Point3D*, Point3D*);
	//! \brief normalize vector
	bool				vectorNormalize		(Point3D*, Point3D*);

	QVector<Point3D>	m_set1;				// Point cloud 1
	QVector<Point3D>	m_set2;				// Point cloud 2

	float				m_delta;			// delta value for 4PCS
	float				m_overlap;			// overlap value for 4PCS

	double				m_mat[4][4];		// transformation matrix

	bool				m_matObtain;		// matrix obtained or not
};
#endif
