// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Tadeusz Jordan
//! \author Wai Khoo
//! \version 2.0
//! \date December 11, 2008
//!
//! \class OpenGLWidget
//! \brief opengl widget display class
//!
//! \file OpenGLWidget.h
//! \brief opengl widget display class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef 	OPENGLWIDGET_H
#define 	OPENGLWIDGET_H

#include 	<QGLWidget>
#include 	<QString>
#include	<QMouseEvent>
#include	<QPoint>

#include	<string>
#include	<fstream>
#include 	<sstream>
using namespace std;

// depth file
struct Vertex {
	double location[3];			//XYZ coordinates
	int colors[3];				// RGB values
};

// OpenGLWidget class
class OpenGLWidget : public QGLWidget
{
		Q_OBJECT
public:
	//! \brief Constructor
		OpenGLWidget			(QWidget *p = 0, QGLWidget *shareWidget = 0);
	//! \brief stores the name of the image
	void	storeImage			(QString, QImage);
	//! \brief store depth image
	void	storeDepth			(QString, string);
	//! \brief clears the display
	void	clear				();
	//! \brief check whether the image is 3D or not
	bool	isDepthImg			();
	//! \brief return a copy of Vertex
	Vertex*	getVertex			();
	//! \brief apply transformation matrix
	void	transformation		(double*, Vertex*, Vertex*);
	//! \brief render transformed point cloud
	void	drawCloud			(Vertex*, Vertex*);

signals:
	//! \brief let Frame class knows that it's dragging
	void	dragging			();
	//! \brief let Frame class knows about the new zoom factor
	void	zoomFactor			(float);

protected:
	//! \brief Initializes OpenGL settings
	void	initializeGL		();
	//! \brief Resizing the image
	void	resizeGL			(int, int);
	//! \brief displaying the image
	void	paintGL				();
	//! \brief mouse press event
	void	mousePressEvent		(QMouseEvent*);
	//! \brief mouse move event
	void	mouseMoveEvent		(QMouseEvent*);
	//! \brief handles key press events
	void	keyPressEvent		(QKeyEvent*);

private:
	//! \brief makes the image a texture
	void	createTexture		();
	//! \brief displays the image mapped to the rectangle
	void	loadImage			();
	//! \brief normalize rotation angle
	void	normalizeAngle		(int*);
	//! \brief set x rotation axis
	void	setXRotation		(int);
	//! \brief set y rotation axis
	void	setYRotation		(int);
	//! \brief set z rotation axis
	void	setZRotation		(int);
	//! \brief make cloud list based on internal Vertex
	GLuint	makeCloud			();
	//! \brief make transformed cloud list based on inputs
	GLuint	makeCloud			(Vertex*, Vertex*);
	//! \brief make cloud list based on inputs
	GLuint	makeTransCloud		(double*, Vertex*, Vertex*);

	GLuint	m_imageTexture;		// texture map of the image
	GLuint	m_ptCloud;			// opengl list for point cloud

	QString	m_imageName;		// image name
	QImage  m_image;			// the image itself

	int	m_xRot;					// x-axis for rotation
	int	m_yRot;					// y-axis for rotation
	int	m_zRot;					// z-axis for rotation
	int	m_translateX;			// x-coordinate for translation
	int	m_translateY;			// y-coordinate for translation
	float	m_scale;			// zoom factor
	QPoint	m_lastPt;			// last point for rotation

	Vertex	*m_vertices;		// struct to store depth points
	bool	m_depthImg;			// flag to indicate whether this is a depth image or not
	bool	m_applyMatrix;		// tell paintgl to apply the current transformation matrix

	double	*m_transformMat;	// transformation matrix for point cloud
};
#endif
