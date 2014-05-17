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
//! \file Handler4PCS.cpp
//! \brief 4PCS handler class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "Handler4PCS.h"

struct tripple {
	int a;
	int b;
	int c;
	tripple(){}
	tripple(int _a, int _b, int _c):a(_a), b(_b), c(_c){}
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constructor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
Handler4PCS::Handler4PCS()
{
	srand		(0);
	m_delta		= 0.9;
	m_overlap	= 0.4;
	m_matObtain	= false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set 4pcs parameters
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief set 4pcs parameters
//! \param[in] delta	delta parameter for 4pcs
//! \param[in] overlap	overlap estimate parameter for 4pcs
void Handler4PCS::setParameters(float delta, float overlap)
{
	m_delta		= delta;
	m_overlap	= overlap;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Store the point clouds
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief store the point clouds
//! \param[in] *v1	1st set of vertices
//! \param[in] *v2	2nd set of vertices
void Handler4PCS::storePoints(Vertex *v1, Vertex *v2)
{
	m_matObtain	= false;

	int size1	= (int)v1[0].location[0];
	int size2	= (int)v2[0].location[0];

	m_set1.clear();
	m_set2.clear();

	for (int i = 1; i < size1 - 1; i++)
		m_set1.push_back(Point3D((double)v1[i].location[0], (double)v1[i].location[1], (double)v1[i].location[2]));

	for (int j = 1; j < size2 - 1; j++)
		m_set2.push_back(Point3D((double)v2[j].location[0], (double)v2[j].location[1], (double)v2[j].location[2]));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Run 4pcs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief run 4pcs
//! \return	return true if 4PCS is success; false otherwise
bool Handler4PCS::run4PCS()
{
	triang(m_set1);
	triang(m_set2);

	PCS4 m_matcher;

	m_matObtain	= m_matcher.compute(m_set1, m_set2, m_delta, m_overlap, m_mat);

	return m_matObtain;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return a copy of the transformed cloud
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return a copy of the transformed cloud
//! \return	copy of the transformed cloud
Vertex* Handler4PCS::getCloud()
{
	Vertex* r	= new Vertex[17000];

	r[0].location[0] = (double)m_set2.size();
	for (int i = 1; i < m_set2.size(); i++)
	{
		r[i].location[0]	= m_set2[i-1].x;
		r[i].location[1]	= m_set2[i-1].y;
		r[i].location[2]	= m_set2[i-1].z;

	}
	return r;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return the transformation matrix
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return the transformation matrix
//! \return	pointer to the 4x4 transformation matrix
double* Handler4PCS::getMatrix()
{
	return &m_mat[0][0];
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Indicate whether it has the matrix or not
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief indicate whether it has the matrix or not
//! \return	true if there's matrix; false otherwise.
bool Handler4PCS::hasMatrix()
{
	return m_matObtain;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Triangulate
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief triangulate
//! \param[in] v	input vector to be triangulated
void Handler4PCS::triang(QVector<Point3D> &v)
{
	Point3D p1, p2, p3, n;
	struct triangulateio in, mid, vorout, out;
	QVector<int> N(v.size());

	in.numberofpoints			= v.size();
	in.numberofpointattributes	= 0;
	in.pointlist				= (REAL*)malloc(in.numberofpoints * 2 * sizeof(REAL));

	for (int i = 0; i < in.numberofpoints; i++)
	{
		in.pointlist[2 * i]		= v[i].x;
		in.pointlist[2 * i + 1]	= v[i].y;
		v[i].n1 = v[i].n2 = v[i].n3 = 0.0;
		N[i]					= 0;
	}

	in.pointattributelist		= NULL;
	in.pointmarkerlist			= NULL;
	in.regionlist				= NULL;

	in.numberofsegments			= 0;
	in.numberofholes			= 0;
	in.numberofregions			= 0;

	mid.pointlist				= (REAL*)NULL;
	mid.pointattributelist		= (REAL*)NULL;
	mid.pointmarkerlist			= (int*)NULL;
	mid.trianglelist			= (int*)NULL;
	mid.triangleattributelist	= (REAL*)NULL;
	mid.neighborlist			= (int*)NULL;
	mid.segmentlist				= (int*)NULL;
	mid.segmentmarkerlist		= (int*)NULL;
	mid.edgelist				= (int*)NULL;
	mid.edgemarkerlist			= (int*)NULL;

	vorout.pointlist			= (REAL*)NULL;
	vorout.pointattributelist	= (REAL*)NULL;
	vorout.edgelist				= (int*)NULL;
	vorout.normlist				= (REAL*)NULL;

	triangulate("QpzAecvn", &in, &mid, &vorout);
	out							= mid;

	for (int i = 0; i < out.numberoftriangles; i++)
	{
		tripple t;
		t.a						= out.trianglelist[i * out.numberofcorners + 0];
		t.b						= out.trianglelist[i * out.numberofcorners + 1];
		t.c						= out.trianglelist[i * out.numberofcorners + 2];

		p1						= v[t.a];
		p2						= v[t.b];
		p3						= v[t.c];

		computeFaceNormal(&p1, &p2, &p3, &n);

		v[t.a].n1				+= n.x;
		v[t.a].n2				+= n.y;
		v[t.a].n3				+= n.z;
		N[t.a]++;

		v[t.b].n1				+= n.x;
		v[t.b].n2				+= n.y;
		v[t.b].n3				+= n.z;
		N[t.b]++;

		v[t.c].n1				+= n.x;
		v[t.c].n2				+= n.y;
		v[t.c].n3				+= n.z;
		N[t.c]++;
	}

	for (int i = 0; i < v.size(); i++)
	{
		if (N[i])
		{
			v[i].n1				/= (float)N[i];
			v[i].n2				/= (float)N[i];
			v[i].n3				/= (float)N[i];

			Point3D u(v[i].n1, v[i].n2, v[i].n3);
			vectorNormalize(&u, &u);

			v[i].n1				= u.x;
			v[i].n2				= u.y;
			v[i].n3				= u.z;
		}
		else
			v[i].n1 = v[i].n2 = v[i].n3 = 0;
	}

	free(in.pointlist);
	free(mid.pointlist);
	free(mid.pointattributelist);
	free(mid.pointmarkerlist);
	free(mid.trianglelist);
	free(vorout.pointlist);
	free(vorout.pointattributelist);
	free(vorout.edgelist);
	free(vorout.normlist);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compute face normal
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief compute face normal
//! \param[in] *p1	1st input Point3D struct
//! \param[in] *p2	2nd input Point3D struct
//! \param[in] *p3	3rd input Point3D struct
//! \param[in, out] *pOut	Point3D struct containing the normal between the inputs.
//! \return	return true if success
bool Handler4PCS::computeFaceNormal(Point3D *p1, Point3D *p2, Point3D *p3, Point3D *pOut)
{
	Point3D a;
	vectorOffset(p3, p2, &a);

	Point3D b;
	vectorOffset(p1, p2, &b);

	Point3D pn;
	vectorGetNormal(&a, &b, &pn);

	return vectorNormalize(&pn, pOut);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compute vector offset
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief compute vector offset
//! \param[in] *pIn	input vector
//! \param[in] *pOffset	offset vector
//! \param[in, out] *pOut input vector offsetted by the offset vector
void Handler4PCS::vectorOffset(Point3D *pIn, Point3D *pOffset, Point3D *pOut)
{
	pOut->x						= pIn->x - pOffset->x;
	pOut->y						= pIn->y - pOffset->y;
	pOut->z						= pIn->z - pOffset->z;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compute vector normal
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief compute vector normal
//! \param[in] *a	1st input vector
//! \param[in] *b	2nd input vector
//! \param[in, out] *pOut	normal vector between 1st and 2nd
void Handler4PCS::vectorGetNormal(Point3D *a, Point3D *b, Point3D *pOut)
{
	pOut->x						= a->y * b->z - a->z * b->y;
	pOut->y						= a->z * b->x - a->x * b->z;
	pOut->z						= a->x * b->y - a->y * b->x;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Normalize vector
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief normalize vector
//! \param[in] *pIn	input Point3D struct containing vertices
//! \param[in, out] *pOut normalized points
//! \return	return true if success
bool Handler4PCS::vectorNormalize(Point3D *pIn, Point3D *pOut)
{
	float len 					= (float)(sqrt(_sqr(pIn->x) + _sqr(pIn->y) + _sqr(pIn->z)));
	if (len)
	{
		pOut->x					= pIn->x / len;
		pOut->y					= pIn->y / len;
		pOut->z					= pIn->z / len;
		return true;
	}
	return false;
}
