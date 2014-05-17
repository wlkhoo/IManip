// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class PCS4
//! \brief 4PCS class
//!
//! \file pcs4.h
//! \brief 4PCS class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef		PCS4_H
#define		PCS4_H

#include 	<QVector>
#include	<QPair>

#include	<boost/numeric/ublas/matrix.hpp>

#include 	"ANN.h"
#include	"ICP.h"
#include 	"vec.h"

#define		FIX_RAND	100
#define		TOO_CLOSE	30
#define		_sqr(x)		((x)*(x))

// ublass definitions
namespace ublas  = boost::numeric::ublas;
typedef ublas::matrix<double, ublas::column_major, ublas::unbounded_array<double> > LA_Fmat;

// holds the base from P
struct QuadIndex{

	int a;
	int b;
	int c;
	int d;
	QuadIndex(int _a,int _b,int _c,int _d):a(_a),b(_b),c(_c),d(_d){}
	QuadIndex(){a=b=c=d=0;}
};

/** The basic 3D point structure. A point contains also directional information that may or may not be used. The directional information can be computed from a mesh or other structures. */
struct Point3D
{
	// coordinates
	double x;
	double y;
	double z;

	// normal
	double n1;
	double n2;
	double n3;
	int a;
	Point3D(double _x,double _y,double _z,int _a):x(_x),y(_y),z(_z),a(_a){n1=n2=n3=0;}
	Point3D(double _x,double _y,double _z):x(_x),y(_y),z(_z),a(0){n1=n2=n3=0;}
	Point3D(){n1=n2=n3=0;}

	bool operator == (const Point3D &t)
	{
		return x==t.x && y==t.y;
	}
	bool operator < (const Point3D &t) const
	{
		if (x<t.x) return true;
		else if (x==t.x && y<t.y) return true;
		else if (x==t.x && y==t.y && z<t.z) return true;
		else return false;
	}
};

/** The class for the computation of the 4PCS algorithm */
class PCS4
{
public:
	PCS4();

	/**
		Compute an approximation of the LCP (directional) from set2 to set1.
		The input sets may or may not contain a normal information for any point.
		The 4PCS algorithm is followed by the ICP of Szymon Rusinkiewicz.

		\param [in] v1 the first input set
		\param [in] v2 the second input set
		\param [in] delta for the LCP (see the paper)
		\param [in] overlapEst estimation of the overlap between the set as a fraction of the size of second set ([0..1])
		\param [out] mat transformation matrix
		\return true on succsess, false on any failure
	*/
	bool compute(QVector<Point3D> &v1, QVector<Point3D> &v2, float delta, float overlapEst, double mat[4][4]);

private:
	void bruteForcePairs(const QVector<Point3D> &v, double d, double dd, double e, QVector<QPair<int, int> > &r);
	void transform(Point3D &u, LA_Fmat &R, double cx, double cy, double cz, double tx, double ty, double tz, float scale = 1.0);
	void transform(QVector<Point3D> &v, LA_Fmat &R, double cx, double cy, double cz, double tx, double ty, double tz, float scale = 1.0);
	void doICP(const QVector<Point3D> &v1, QVector<Point3D> &v2, xform &m);

	double verify(const QVector<Point3D> &v1, double eps, LA_Fmat &R, double bestf, double cx, double cy, double cz, double tx, double ty, double tz, float scale);
	double meanDist(QVector<Point3D> &v);
	double PointsDistance(const Point3D &p, const Point3D &q);
	double computeBestRigid(QVector<QPair<Point3D, Point3D> > &pairs, LA_Fmat &R, double &tx, double &ty, double &tz, double &cx, double &cy, double &cz, float &scale);

	bool align(QVector<Point3D> &v1, QVector<Point3D> &v2, float eps, LA_Fmat &rMat);
	bool tryOne(QVector<Point3D> &m1, QVector<Point3D> &m2, QVector<Point3D> &list1, QVector<Point3D> &list2, QVector<Point3D> &trList, QVector<Point3D> &trCpy, float &bestf, float eps, LA_Fmat &rMat);
	bool selectQuad(const QVector<Point3D> &v, QVector<Point3D> &quad, double &f1, double &f2, int &a, int &b, int &c, int &d);
	bool selectRandomDiam(const QVector<Point3D> &v, int &a, int &b, int &c);
	bool tryQuad(const QVector<Point3D> &v, QVector<Point3D> &quad, double &f1, double &f2, float e);
	bool findQuads(const QVector<Point3D> &v, QVector<Point3D> &quad, QVector<QPair<int, int> > &r1, QVector<QPair<int, int> > &r2, double f1, double f2, double e, double e1, QVector<QuadIndex> &ret);
	bool checkQuad(const Point3D &p1, const Point3D &p2, const Point3D &p3, const Point3D &p4, float d1, float d2, float d3, float d4, float e);

	float dist3D_Segment_to_Segment(point &p1, point &p2, point &q1, point &q2, float &f1, float &f2);

	float 			m_app;
	float 			m_normDiff;
	float 			m_qd;
	float 			m_meanDist0;
	float 			m_estFrac;
	float 			m_bcx, m_bcy, m_bcz;
	float 			m_btx, m_bty, m_btz;
	float 			m_bestScale;
	float 			m_thr;

	int 			m_numTry;
	int 			m_useNormals;
	int 			m_sample;

	QVector<Point3D> 	m_list1;
	QVector<Point3D> 	m_list2;
	QVector<Point3D> 	m_cpy1;
	QVector<Point3D> 	m_cpy2;
	QVector<Point3D> 	m_quad;

	ANNkd_tree 		*m_the_tree;
	ANNpointArray 		m_data_pts0;
};
#endif
