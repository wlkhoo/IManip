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
//! \file pcs4.cpp
//! \brief 4PCS class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include			"pcs4.h"

#include			<QMessageBox>
#include			<QObject>
#include			<QString>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constructor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PCS4::PCS4()
{
	m_the_tree 		= 0;
	m_data_pts0 	= 0;
	m_app 			= 0.0;
	m_useNormals 	= 1;
	m_normDiff		= 0.02;
	m_qd			= -1.0;
	m_numTry		= -1;
	m_meanDist0		= 1.0;
	m_estFrac		= 0.4;
	m_bestScale		= 1.0;
	m_thr			= -1.0;
	m_sample		= 500;
	m_quad.resize	(4);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Main function - compute 4PCS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::compute(QVector<Point3D> &v1, QVector<Point3D> &v2, float delta, float overlapEst, double mat[4][4])
{
	m_estFrac		= overlapEst;

	double xc1 = 0.0;
	double yc1 = 0.0;
	double zc1 = 0.0;
	double xc2 = 0.0;
	double yc2 = 0.0;
	double zc2 = 0.0;

	for (int i = 0; i < v1.size(); i++)
	{
		xc1 	+= v1[i].x;
		yc1 	+= v1[i].y;
		zc1 	+= v1[i].z;
	}

	xc1 		/= v1.size();
	yc1 		/= v1.size();
	zc1 		/= v1.size();

	for (int i = 0; i < v2.size(); i++)
	{
		xc2 	+= v2[i].x;
		yc2 	+= v2[i].y;
		zc2 	+= v2[i].z;
	}

	xc2 		/= v2.size();
	yc2 		/= v2.size();
	zc2 		/= v2.size();

	m_list1.clear	();
	m_list2.clear	();

	int s1 		= v1.size()/m_sample;
	int s2 		= v2.size()/m_sample;

	for (int i = 0; i < v1.size(); i++)
	{
		if (rand() % s1 == 0)
			m_list1.push_back(v1[i]);
	}

	for (int i = 0; i < v2.size(); i++)
	{
		if (rand() % s2 == 0)
			m_list2.push_back(v2[i]);
	}

	for (int i = 0; i < m_list1.size(); i++)
	{
		m_list1[i].x	-= xc1;
		m_list1[i].y	-= yc1;
		m_list1[i].z 	-= zc1;
	}

	for (int i = 0; i < m_list2.size(); i++)
	{
		m_list2[i].x	-= xc2;
		m_list2[i].y	-= yc2;
		m_list2[i].z	-= zc2;
	}

	int n_pts		= m_list1.size();
	if (m_data_pts0)
		annDeallocPts	(m_data_pts0);
	if (m_the_tree)
		delete			m_the_tree;

	m_data_pts0		= annAllocPts(n_pts, 3);
	for (int i = 0; i < m_list1.size(); i++)
	{
		m_data_pts0[i][0]	= m_list1[i].x;
		m_data_pts0[i][1]	= m_list1[i].y;
		m_data_pts0[i][2]	= m_list1[i].z;
	}

	m_the_tree		= new ANNkd_tree(m_data_pts0, n_pts, 3);
	m_meanDist0		= meanDist(m_list1)/2.0;

	float xx1, yy1, zz1, xx2, yy2, zz2;
	xx1 = yy1 = zz1 = FLT_MAX;
	xx2 = yy2 = zz2 = -FLT_MAX;

	for (int i = 0; i < m_list2.size(); i++)
	{
		if (m_list2[i].x < xx1)
			xx1 	= m_list2[i].x;
		if (m_list2[i].x > xx2)
			xx2		= m_list2[i].x;
		if (m_list2[i].y < yy1)
			yy1		= m_list2[i].y;
		if (m_list2[i].y > yy2)
			yy2 	= m_list2[i].y;
		if (m_list2[i].z < zz1)
			zz1		= m_list2[i].z;
		if (m_list2[i].z > zz2)
			zz2		= m_list2[i].z;
	}

	float dx		= xx2 - xx1;
	float dy		= yy2 - yy1;
	float dz		= zz2 - zz1;
	float diam		= max(dx, max(dy, dz));

	if (diam > 240)
		diam		= 240;

	delta			= m_meanDist0 * delta;
	m_qd			= diam * m_estFrac * 2.0;

	double u		= log(0.00001)/log(1.0-pow((double)m_estFrac, 4.0));
	m_numTry		= (int)(u * 80.0/m_qd);
	m_thr			= m_estFrac;

	if (m_numTry < 4)
		m_numTry		= 4;

	m_cpy1			= m_list1;
	m_cpy2			= m_list2;

	LA_Fmat			rMat(3, 3);

	if (!align(m_list1, m_list2, delta, rMat))
	{
		QMessageBox::information(0, QObject::tr("4PCS"), QObject::tr("Alignment returns false.\nTerminated."));
		return false;
	}

	Point3D			p(m_bcx + xc2, m_bcy + yc2, m_bcz + zc2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rMat[i][j]	*= m_bestScale;
		}
	}

	transform(p, rMat, 0, 0, 0, 0, 0, 0);

	QVector<Point3D> cv2	= v2;

	transform(v2, rMat, 0, 0, 0, m_bcx-p.x+m_btx+xc1, m_bcy-p.y+m_bty+yc1, m_bcz-p.z+m_btz+zc1);

	LA_Fmat omat(4,4);
	LA_Fmat cmat(4,4);

	omat[0][0]	= rMat[0][0];
	omat[0][1]	= rMat[0][1];
	omat[0][2]	= rMat[0][2];
	omat[0][3]	= m_bcx-p.x+m_btx+xc1;

	omat[1][0]	= rMat[1][0];
	omat[1][1]	= rMat[1][1];
	omat[1][2]	= rMat[1][2];
	omat[1][3]	= m_bcy-p.y+m_bty+yc1;

	omat[2][0]	= rMat[2][0];
	omat[2][1]	= rMat[2][1];
	omat[2][2]	= rMat[2][2];
	omat[2][3]	= m_bcz-p.z+m_btz+zc1;

	omat[3][0]	= 0.0;
	omat[3][1]	= 0.0;
	omat[3][2]	= 0.0;
	omat[3][3]	= 1.0;

	xform		m;
	doICP(v1, v2, m);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cmat[i][j]	= m[j*4+i];
		}
	}

	LA_Fmat R	= ublas::prod(cmat, omat);

	m[0]	= mat[0][0]	= R[0][0];
	m[4]	= mat[0][1]	= R[0][1];
	m[8]	= mat[0][2]	= R[0][2];
	m[12]	= mat[0][3]	= R[0][3];

	m[1]	= mat[1][0]	= R[1][0];
	m[5]	= mat[1][1]	= R[1][1];
	m[9]	= mat[1][2]	= R[1][2];
	m[13]	= mat[1][3]	= R[1][3];

	m[2]	= mat[2][0]	= R[2][0];
	m[6]	= mat[2][1]	= R[2][1];
	m[10]	= mat[2][2]	= R[2][2];
	m[14]	= mat[2][3]	= R[2][3];

	m[3]	= mat[3][0]	= 0;
	m[7]	= mat[3][1]	= 0;
	m[11]	= mat[3][2]	= 0;
	m[15]	= mat[3][3]	= 1;

	for (int i = 0; i < cv2.size(); i++)
	{
		LA_Fmat mm(4,1);
		LA_Fmat pp;

		mm[0][0] = cv2[i].x;
		mm[1][0] = cv2[i].y;
		mm[2][0] = cv2[i].z;
		mm[3][0] = 1;
		pp			= ublas::prod(R, mm);

		cv2[i].x	= pp[0][0];
		cv2[i].y	= pp[1][0];
		cv2[i].z	= pp[2][0];
	}

	v2		= cv2;

	return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Align
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::align(QVector<Point3D> &v1, QVector<Point3D> &v2, float eps, LA_Fmat &rMat)
{
	QVector<Point3D> listOut, cpyOut, tt, tt1, m1, m2;
	float f;
	float bestf	= 0.0;
	bool ok;

	m1	= v1;
	m2	= v2;

	for (int i = 0; i < m_numTry; i++)
	{
		f	= bestf;
		ok	= tryOne(m1, m2, v1, v2, tt, tt1, f, eps, rMat);
		if (f > bestf)
		{
			bestf	= f;
			listOut	= tt;
			cpyOut	= tt1;
		}
		if (ok)
			break;
	}
	return ok;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Try one
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::tryOne(QVector<Point3D> &m1, QVector<Point3D> &m2, QVector<Point3D> &list1, QVector<Point3D> &list2, QVector<Point3D> &trList, QVector<Point3D> &trCpy, float &bestf, float eps, LA_Fmat &rMat)
{
	double tx, ty, tz, f1, f2;
	int id1, id2, id3, id4;
	QVector<QPair<Point3D, Point3D> > pr(4);
	QVector<QPair<int ,int> > r1, r2;
	QVector<QuadIndex> ret;

	if (!selectQuad(m1, m_quad, f1, f2, id1, id2, id3, id4))
		return false;

	double d1	= PointsDistance(m_quad[0], m_quad[1]);
	double d2	= PointsDistance(m_quad[2], m_quad[3]);
	double dd1	= PointsDistance(Point3D(m_quad[0].n1, m_quad[0].n2, m_quad[0].n3), Point3D(m_quad[1].n1, m_quad[1].n2, m_quad[1].n3));
	double dd2	= PointsDistance(Point3D(m_quad[2].n1, m_quad[2].n2, m_quad[2].n3), Point3D(m_quad[3].n1, m_quad[3].n2, m_quad[3].n3));

	bruteForcePairs(m2, d1, dd1, eps, r1);
	bruteForcePairs(m2, d2, dd2, eps, r2);

	if (r1.size() == 0 || r2.size() == 0)
		return false;

	if (!findQuads(m2, m_quad, r1, r2, f1, f2, eps*2, eps, ret))
		return false;

	LA_Fmat R(3,3), R1(3,3), R0(3,3);
	int A	= ret.size();

	for (int i = 0; i < A; i++)
	{
		pr.resize(4);
		int a1		= ret[i].a;
		int b1		= ret[i].b;
		int c1		= ret[i].c;
		int d1		= ret[i].d;

		pr[0].first	= m1[id1];
		pr[0].second= m2[a1];
		pr[1].first	= m1[id2];
		pr[1].second= m2[b1];
		pr[2].first	= m1[id3];
		pr[2].second= m2[c1];
		pr[3].first	= m1[id4];
		pr[3].second= m2[d1];

		double cx, cy, cz, f;
		float scale = 1.0;

		f			= computeBestRigid(pr, R, tx, ty, tz, cx, cy, cz, scale);

		if (f < 5 * eps)
		{
			f		= verify(list2, m_meanDist0 * eps * 2.0, R, bestf, cx, cy, cz, tx, ty, tz, scale);
			if (f > bestf)
			{
				bestf		= f;
				trList		= list2;
				transform(trList, R, cx, cy, cz, tx, ty, tz, scale);
				trCpy		= m_cpy2;
				transform(trCpy, R, cx, cy, cz, tx, ty, tz, scale);
				rMat			= R;
				m_bcx			= cx;
				m_bcy			= cy;
				m_bcz			= cz;
				m_btx			= tx;
				m_bty			= ty;
				m_btz			= tz;
				m_bestScale		= scale;
			}
			if (bestf > m_thr)
				return true;
			else
				return false;
		}
	}
	return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Brute force pairs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PCS4::bruteForcePairs(const QVector<Point3D> &v, double d, double dd, double e, QVector<QPair<int, int> > &r)
{
	double t, t1;

	r.clear				();
	r.reserve			(2 * v.size());

	for (int j = 0; j < v.size(); j++)
	{
		const Point3D &p	= v[j];

		for (int i = j + 1; i < v.size(); i++)
		{
			t			= sqrt(_sqr(v[i].x - p.x) + _sqr(v[i].y - p.y) + _sqr(v[i].z - p.z));
			if (m_useNormals)
				t1		= sqrt(_sqr(v[i].n1 - p.n1) + _sqr(v[i].n2 - p.n2) + _sqr(v[i].n3 - p.n3));
			else
				t1		= dd;

			if (fabs(t - d) < e && fabs(t1 - dd) < m_normDiff)
			{
				r.push_back(QPair<int, int>(j, i));
				r.push_back(QPair<int, int>(i, j));
			}
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Verify
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double PCS4::verify(const QVector<Point3D> &v1, double eps, LA_Fmat &R, double bestf, double cx, double cy, double cz, double tx, double ty, double tz, float scale)
{
	int 			s, a;
	float 			rnd;
	Point3D 		p;
	ANNpoint 		query_pt;
	ANNidxArray		nn_idx;
	ANNdistArray	dists;

	query_pt		= annAllocPt(3);
	nn_idx			= new ANNidx[1];
	dists			= new ANNdist[1];

	s 				= 0;
	rnd				= (float)v1.size();
	a				= (int)(bestf * rnd);

	for (int i = 0; i < rnd; i++)
	{
		p			= v1[i];
		transform	(p, R, cx, cy, cz, tx, ty, tz, scale);

		query_pt[0]	= p.x;
		query_pt[1]	= p.y;
		query_pt[2]	= p.z;

		m_the_tree	->annkSearch(query_pt, 1, nn_idx, dists, m_app);

		if (dists[0] < eps)
			s++;
		if ((rnd - i + s) < a)
			return (float)s/(float)rnd;
	}

	annDeallocPt	(query_pt);
	delete []		nn_idx;
	delete []		dists;

	return (float)s/(float)rnd;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Mean distance
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double PCS4::meanDist(QVector<Point3D> &v)
{
	srand			(0);
	float d			= 0.0;
	int n			= 0;
	ANNpoint query_pt;
	ANNidxArray nn_idx;
	ANNdistArray dists;

	query_pt		= annAllocPt(3);
	nn_idx			= new ANNidx[2];
	dists			= new ANNdist[2];

	for (int i = 0; i < FIX_RAND * 10; i++)
	{
		int k		= rand() % v.size();

		query_pt[0]	= v[k].x;
		query_pt[1]	= v[k].y;
		query_pt[2]	= v[k].z;

		m_the_tree	->annkSearch(query_pt, 2, nn_idx, dists, m_app);

		if (dists[1] < TOO_CLOSE)
		{
			d		+= dists[1];
			n++;
		}
	}

	annDeallocPt	(query_pt);
	delete []		nn_idx;
	delete []		dists;

	return d/(float)n;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Select quad
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::selectQuad(const QVector<Point3D> &v, QVector<Point3D> &quad, double &f1, double &f2, int &a, int &b, int &c, int &d)
{
	double x1, y1, z1, x2, y2, z2, x3, y3, z3;
	double X, A, B, C;
	double bestv, di, d0, d1, d2, d3;
	int tr = 0;
	while (tr < FIX_RAND)
	{
		if (!selectRandomDiam(v, a, b, c))
			return false;

		quad[0]		= v[a];
		quad[1]		= v[b];
		quad[2]		= v[c];

		x1			= quad[0].x;
		y1			= quad[0].y;
		z1			= quad[0].z;
		x2			= quad[1].x;
		y2			= quad[1].y;
		z2			= quad[1].z;
		x3			= quad[2].x;
		y3			= quad[2].y;
		z3			= quad[2].z;

		X			= ((-x3*y2*z1) + (x2*y3*z1) + (x3*y1*z2) - (x1*y3*z2) - (x2*y1*z3) + (x1*y2*z3));

		Point3D cg	((x1+x2+x3)/3, (y1+y2+y3)/3, (z1+z2+z3));

		bestv		= FLT_MAX;
		if (X != 0)
		{
			A		= (-y2*z1 + y3*z1 + y1*z2 - y3*z2 - y1*z3 + y2*z3)/X;
			B		= (x2*z1 - x3*z1 - x1*z2 + x3*z2 + x1*z3 - x2*z3)/X;
			C		= (-x2*y1 + x3*y1 + x1*y2 - x3*y2 - x1*y3 + x2*y3)/X;
			d		= -1;
			bestv	= 10000000;

			for (int i = 0; i < v.size(); i++)
			{
				d0	= PointsDistance(v[i], cg);
				d1	= PointsDistance(v[i], v[a]);
				d2	= PointsDistance(v[i], v[b]);
				d3	= PointsDistance(v[i], v[c]);

				if (d1 < TOO_CLOSE/3 || d2 < TOO_CLOSE/3 || d3 < TOO_CLOSE/3)
					continue;

				di	= fabs(A*v[i].x + B*v[i].y + C*v[i].z - 1.0);
				if (di < bestv)
				{
					bestv	= di;
					d		= i;
				}
			}

			if (d != -1)
			{
				quad[3]	= v[d];
				if (tryQuad(v, quad, f1, f2, 2*bestv))
					return true;
			}
		}
		tr++;
	}
	return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Select random diam
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::selectRandomDiam(const QVector<Point3D> &v, int &a, int &b, int &c)
{
	float d, dt;
	int n, at, bt, ct;

	d		= 0.0;
	n		= v.size();
	a = b = c = -1;
	at		= rand() % n;

	for (int i = 0; i < FIX_RAND; i++)
	{
		bt	= rand() % n;
		ct	= rand() % n;

		point u(v[bt].x - v[at].x, v[bt].y - v[at].y, v[bt].z - v[at].z);
		point w(v[ct].x - v[at].x, v[ct].y - v[at].y, v[ct].z - v[at].z);

		dt	= 0.5 * len(u CROSS w);

		if (dt > d && len(u) < m_qd && len(w) < m_qd)
		{
			a	= at;
			b 	= bt;
			c	= ct;
			d	= dt;
		}
	}

	if (a == -1 || b == -1 || c == -1)
		return false;
	else
		return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Try quad
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::tryQuad(const QVector<Point3D> &v, QVector<Point3D> &quad, double &f1, double &f2, float e)
{
	QVector<point> nq;
	nq.push_back(point(quad[0].x, quad[0].y, quad[0].z));
	nq.push_back(point(quad[1].x, quad[1].y, quad[1].z));
	nq.push_back(point(quad[2].x, quad[2].y, quad[2].z));
	nq.push_back(point(quad[3].x, quad[3].y, quad[3].z));

	float minv, u1, u2, f;
	int b1, b2, b3, b4, k, l;

	minv	= FLT_MAX;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
				continue;

			k	= l = 0;

			while (k == i || k == j)
				k++;
			while (l == i || l == j || l == k)
				l++;

			f	= dist3D_Segment_to_Segment(nq[i], nq[j], nq[k], nq[l], u1, u2);
			if (f < minv)
			{
				minv	= f;
				b1		= i;
				b2		= j;
				b3		= k;
				b4		= l;
				f1		= u1;
				f2		= u2;
			}
		}
	}

	QVector<Point3D> tmp	= quad;
	quad[0]				= tmp[b1];
	quad[1]				= tmp[b2];
	quad[2]				= tmp[b3];
	quad[3]				= tmp[b4];

	return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Find quads
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::findQuads(const QVector<Point3D> &v, QVector<Point3D> &quad, QVector<QPair<int, int> > &r1, QVector<QPair<int, int> > &r2, double f1, double f2, double e, double e1, QVector<QuadIndex> &ret)
{
	int n_pts			= 2 * r1.size();
	int n				= n_pts;
	ANNkd_tree			*tree;
	ANNpointArray		data_pts = annAllocPts(n_pts, 3);

	ANNpoint			query_pt;
	ANNidxArray			nn_idx;
	ANNdistArray		dists;

	query_pt			= annAllocPt(3);
	nn_idx				= new ANNidx[n];
	dists				= new ANNdist[n];

	ret.clear			();

	float d1			= PointsDistance(quad[0], quad[2]);
	float d2			= PointsDistance(quad[0], quad[3]);
	float d3			= PointsDistance(quad[1], quad[2]);
	float d4			= PointsDistance(quad[1], quad[3]);

	for (int i = 0; i < r1.size(); i++)
	{
		const Point3D &p1	= v[r1[i].first];
		const Point3D &p2	= v[r1[i].second];

		data_pts[i*2][0]	= p1.x + f1 * (p2.x - p1.x);
		data_pts[i*2][1]	= p1.y + f1 * (p2.y - p1.y);
		data_pts[i*2][2]	= p1.z + f1 * (p2.z - p1.z);
		data_pts[i*2+1][0]	= p1.x + f2 * (p2.x - p1.x);
		data_pts[i*2+1][1]	= p1.y + f2 * (p2.y - p1.y);
		data_pts[i*2+1][2]	= p1.z + f2 * (p2.z - p1.z);
	}

	tree				= new ANNkd_tree(data_pts, n_pts, 3);

	for (int i = 0; i < r2.size(); i++)
	{
		const Point3D &p1	= v[r2[i].first];
		const Point3D &p2	= v[r2[i].second];

		query_pt[0]			= p1.x + f1 * (p2.x - p1.x);
		query_pt[1]			= p1.y + f1 * (p2.y - p1.y);
		query_pt[2]			= p1.z + f1 * (p2.z - p1.z);

		tree				->annkFRSearch(query_pt, e1, n, nn_idx, dists, m_app);

		for (int j = 0; j < n; j++)
		{
			if (dists[j] != ANN_DIST_INF)
			{
				int id		= nn_idx[j]/2;
				if (checkQuad(v[r2[i].first], v[r2[i].second], v[r1[id].first], v[r1[id].second], d1, d2, d3, d4, e))
				{
					QuadIndex quad1(r1[id].first, r1[id].second, r2[i].first, r2[i].second);
					ret.push_back(quad1);
				}
			}
			else
				break;
		}

		query_pt[0]			= p1.x + f2 * (p2.x - p1.x);
		query_pt[1]			= p1.y + f2 * (p2.y - p1.y);
		query_pt[2]			= p1.z + f2 * (p2.z - p1.z);
		tree				->annkFRSearch(query_pt, e1, n, nn_idx, dists, m_app);

		for (int j = 0; j < n; j++)
		{
			if (dists[j] != ANN_DIST_INF)
			{
				int id		= nn_idx[j]/2;
				if (checkQuad(v[r2[i].first], v[r2[i].second], v[r1[id].first], v[r1[id].second], d1, d2, d3, d4, e))
				{
					QuadIndex quad2(r1[id].first, r1[id].second, r2[i].first, r2[i].second);
					ret.push_back(quad2);
				}
			}
			else
				break;
		}
	}

	annDeallocPt			(query_pt);
	annDeallocPts			(data_pts);
	delete []				nn_idx;
	delete []				dists;
	delete					tree;

	return ret.size() != 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ICP
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PCS4::doICP(const QVector<Point3D> &v1, QVector<Point3D> &v2, xform &m)
{
	TriMesh *mesh1			= new TriMesh();
	TriMesh *mesh2			= new TriMesh();

	int i;
	for(i=0; i<v1.size(); i++)
		mesh1->vertices.push_back(point(v1[i].x,v1[i].y,v1[i].z));

	for(i=0; i<v2.size(); i++)
		mesh2->vertices.push_back(point(v2[i].x,v2[i].y,v2[i].z));


	xform xf1, xf2;

	mesh1					->need_normals();
	mesh2					->need_normals();
	mesh1					->need_neighbors();
	mesh2					->need_neighbors();
	mesh1					->need_adjacentfaces();
	mesh2					->need_adjacentfaces();


	float err				= ICP(mesh1, mesh2, xf1, xf2, 0, true, false);

	m						= xf2;

	delete					mesh1;
	delete					mesh2;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Point distance
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double PCS4::PointsDistance(const Point3D &p, const Point3D &q)
{
	return sqrt(_sqr(p.x - q.x) + _sqr(p.y - q.y) + _sqr(p.z - q.z));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compute best rigid
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double PCS4::computeBestRigid(QVector<QPair<Point3D, Point3D> > &pairs, LA_Fmat &R, double &tx, double &ty, double &tz, double &cx, double &cy, double &cz, float &scale)
{
	double x1, y1, z1, x2, y2, z2;
	x1 = y1 = z1 = x2 = y2 = z2 = 0.0;

	for (int i = 0; i < pairs.size(); i++)
	{
		const Point3D &p	= pairs[i].first;
		const Point3D &q	= pairs[i].second;

		x1					+= p.x;
		y1					+= p.y;
		z1					+= p.z;
		x2					+= q.x;
		y2					+= q.y;
		z2					+= q.z;
	}

	x1						/= pairs.size();
	y1						/= pairs.size();
	z1						/= pairs.size();
	x2						/= pairs.size();
	y2						/= pairs.size();
	z2						/= pairs.size();

	point centroid1			(x1, y1, z1);
	point centroid2			(x2, y2, z2);

	double u1, u2, l1, l2, t;
	QVector<QPair<Point3D, Point3D> > tp = pairs;

	u1 = u2 = 0;
	scale 					= 1.0;

	for (int i = 0; i < pairs.size(); i++)
	{
		double d			= FLT_MAX;
		int best;

		for (int j = 0; j < pairs.size(); j++)
		{
			point s1		(tp[i].first.x, tp[i].first.y, tp[i].first.z);
			point s2		(tp[j].second.x, tp[j].second.y, tp[j].second.z);

			l1				= dist2(s1, centroid1);
			l2				= dist2(s2, centroid2);

			t				= fabs(l1 - l2);
			if (t < d)
			{
				d			= t;
				best		= j;
			}
		}
		pairs[i].second		= tp[best].second;
	}

	point rl1 (pairs[0].first.x, pairs[0].first.y, pairs[0].first.z);
	point rl2 (pairs[1].first.x, pairs[1].first.y, pairs[1].first.z);
	point rl3 (pairs[2].first.x, pairs[2].first.y, pairs[2].first.z);
	point rr1 (pairs[0].second.x, pairs[0].second.y, pairs[0].second.z);
	point rr2 (pairs[1].second.x, pairs[1].second.y, pairs[1].second.z);
	point rr3 (pairs[2].second.x, pairs[2].second.y, pairs[2].second.z);

	centroid1				= rl1 + rl2 + rl3;
	centroid1				/= 3.0;
	centroid2				= rr1 + rr2 + rr3;
	centroid2				/= 3.0;

	point xl				= rl2 - rl1;
	normalize				(xl);

	point yl				= (rl3 - rl1) - ((rl3-rl1) DOT xl) * xl;
	normalize				(yl);

	point zl				= xl CROSS yl;

	point xr				= rr2 - rr1;
	normalize				(xr);

	point yr				= (rr3 - rr1) - ((rr3 - rr1) DOT xr) * xr;
	normalize				(yr);

	point zr				= xr CROSS yr;

	LA_Fmat Tl(3,3);
	Tl[0][0]				= xl[0];
	Tl[0][1]				= xl[1];
	Tl[0][2]				= xl[2];
	Tl[1][0]				= yl[0];
	Tl[1][1]				= yl[1];
	Tl[1][2]				= yl[2];
	Tl[2][0]				= zl[0];
	Tl[2][1]				= zl[1];
	Tl[2][2]				= zl[2];

	LA_Fmat Tr(3,3);
	Tr[0][0]				= xr[0];
	Tr[0][1]				= xr[1];
	Tr[0][2]				= xr[2];
	Tr[1][0]				= yr[0];
	Tr[1][1]				= yr[1];
	Tr[1][2]				= yr[2];
	Tr[2][0]				= zr[0];
	Tr[2][1]				= zr[1];
	Tr[2][2]				= zr[2];

	LA_Fmat Tlt				= ublas::trans(Tl);
	R						= ublas::prod(Tlt, Tr);

	cx						= centroid2[0];
	cy						= centroid2[1];
	cz						= centroid2[2];
	tx						= centroid1[0] - centroid2[0];
	ty						= centroid1[1] - centroid2[1];
	tz						= centroid1[2] - centroid2[2];

	LA_Fmat mm(3, 1), pp;
	double u				= 0.0;

	for (int i = 0; i < 3; i++)
	{
		mm[0][0]			= pairs[i].second.x - centroid2[0];
		mm[1][0]			= pairs[i].second.y - centroid2[1];
		mm[2][0]			= pairs[i].second.z - centroid2[2];

		pp					= ublas::prod(R, mm);
		u					+= sqrt(_sqr(pp[0][0] - (pairs[i].first.x - centroid1[0])) +
								_sqr(pp[1][0] - (pairs[i].first.y - centroid1[1])) +
								_sqr(pp[2][0] - (pairs[i].first.z - centroid1[2])));
	}

	u						/= pairs.size();

	return					u;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Transform
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PCS4::transform(Point3D &u, LA_Fmat &R, double cx, double cy, double cz, double tx, double ty, double tz, float scale)
{
	LA_Fmat p(3, 1), pp;

	p[0][0]					= (u.x - cx) * scale;
	p[1][0]					= (u.y - cy) * scale;
	p[2][0]					= (u.z - cz) * scale;

	pp						= ublas::prod(R, p);
	u.x						= pp[0][0] + cx + tx;
	u.y						= pp[1][0] + cy + ty;
	u.z						= pp[2][0] + cz + tz;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Transform (overloaded)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PCS4::transform(QVector<Point3D> &v, LA_Fmat &R, double cx, double cy, double cz, double tx, double ty, double tz, float scale)
{
	LA_Fmat p(3, 1), pp;

	for (int i = 0; i < v.size(); i++)
	{
		p[0][0]				= (v[i].x - cx) * scale;
		p[1][0]				= (v[i].y - cy) * scale;
		p[2][0]				= (v[i].z - cz) * scale;

		pp					= ublas::prod(R, p);

		v[i].x				= pp[0][0] + cx + tx;
		v[i].y				= pp[1][0] + cy + ty;
		v[i].z				= pp[2][0] + cz + tz;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 3D distance from segment to segment
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float PCS4::dist3D_Segment_to_Segment(point &p1, point &p2, point &q1, point &q2, float &f1, float &f2)
{
	point u					= p2 - p1;
	point v					= q2 - q1;
	point w					= p1 - q1;

	float a, b, c, d, e, D, sc, sN, sD, tc, tN, tD;
	a						= u DOT u;
	b						= u DOT v;
	c						= v DOT v;
	d						= u DOT w;
	e						= v DOT w;
	D						= a*c - b*b;
	sD						= tD = D;

	if (D < 0.0001)
	{
		sN					= 0.0;
		sD					= 1.0;
		tN					= e;
		tD					= c;
	}
	else
	{
		sN					= (b*e - c*d);
		tN					= (a*e - b*d);
		if (sN < 0.0)
		{
			sN				= 0.0;
			tN				= e;
			tD				= c;
		}
		else if (sN > sD)
		{
			sN				= sD;
			tN				= e + b;
			tD				= c;
		}
	}

	if (tN < 0.0)
	{
		tN					= 0.0;
		if (-d < 0.0)
			sN				= 0.0;
		else if (-d > a)
			sN				= sD;
		else
		{
			sN				= -d;
			sD				= a;
		}
	}
	else if (tN > tD)
	{
		tN					= tD;
		if ((-d + b) < 0.0)
			sN				= 0.0;
		else if ((-d + b) > a)
			sN				= sD;
		else
		{
			sN				= (-d + b);
			sD				= a;
		}
	}

	sc						= (abs((int)sN) < 0.0001 ? 0.0 : sN/sD);
	tc						= (abs((int)tN) < 0.0001 ? 0.0 : tN/tD);

	point dP				= w + (sc*u) - (tc*v);

	f1						= sc;
	f2						= tc;

	return len(dP);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check quad
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool PCS4::checkQuad(const Point3D &p1, const Point3D &p2, const Point3D &p3, const Point3D &p4, float d1, float d2, float d3, float d4, float e)
{
	float dd1				= PointsDistance(p1, p3);
	float dd2				= PointsDistance(p1, p4);
	float dd3				= PointsDistance(p2, p3);
	float dd4				= PointsDistance(p2, p4);

	if ((fabs(d1-dd1) < 2*e) && (fabs(d2-dd3) < 2*e) && (fabs(d3-dd2) < 2*e) && (fabs(d4-dd4) < 2*e))
		return true;
	else
		return false;
}
