// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IManip: Image Manipulator
//
//! \author Wai Khoo
//! \author Tadeusz Jordan
//! \version 2.0
//! \date December 11, 2008
//!
//! \class OpenGLWidget
//! \brief OpenGLWidget implementation
//!
//! \file OpenGLWidget.cpp
//! \brief opengl widget display class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "OpenGLWidget.h"
#include <cmath>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Constructor
//! \details init opengl
//! \param[in] *p Qwidet
//! \param[in] *shareWidget QGLWidget
OpenGLWidget::OpenGLWidget(QWidget *p, QGLWidget *shareWidget)
	: QGLWidget(p, shareWidget)
{
	initializeGL();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initializes OpenGL settings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Initializes OpenGL settings
void OpenGLWidget::initializeGL()
{
	m_vertices	= new Vertex[17000];
	m_scale		= 1.0;
	m_xRot		= 0;
	m_yRot		= 0;
	m_zRot		= 0;
	m_translateX	= 0;
	m_translateY	= 0;
	m_depthImg	= false;
	m_applyMatrix	= false;
	glEnable	(GL_DEPTH_TEST);
	glEnable	(GL_TEXTURE_2D);
	glEnable	(GL_POINT_SMOOTH);
	glEnable	(GL_BLEND);
	glEnable	(GL_CULL_FACE);
	glEnable	(GL_LINE_SMOOTH);
	glClearColor	(0.0, 0.0, 0.0, 1.0);
	glPixelStorei	(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resizing the image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief Resizing the image
//! \param[in] *width int
//! \param[in] *height int
void OpenGLWidget::resizeGL(int width, int height)
{
	glViewport	(0, 0, width, height);
	glMatrixMode	(GL_PROJECTION);
	glLoadIdentity	();
	glOrtho		(-10.0, 10.0, -10.0, 10.0, -500.0, 500.0);
	glMatrixMode	(GL_MODELVIEW);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// displaying the image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief displaying the image
void OpenGLWidget::paintGL()
{
	glClear			(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_applyMatrix)
		glLoadMatrixd	(m_transformMat);
	else
		glLoadIdentity	();

	glTranslatef	((float)m_translateX, (float)m_translateY, 0.0);
	glScalef		(m_scale, m_scale, m_scale);
	// make 1 degree turn for every 8 pixels displaced
	glRotated		(m_xRot/8.0, 1.0, 0.0, 0.0);	// rotate x-axis
	glRotated		(m_yRot/8.0, 0.0, 1.0, 0.0);	// rotate y-axis
	glRotated		(m_zRot/8.0, 0.0, 0.0, 1.0);	// rotate z-axis

	if (m_depthImg)
		glCallList(m_ptCloud);
	else
	{
		createTexture	();
		loadImage	();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// clears the display
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief clears the display
void OpenGLWidget::clear()
{
	deleteTexture	(m_imageTexture);
	glDeleteLists	(m_ptCloud, 1);
	m_image			= QImage();
	m_scale			= 1.0;
	m_xRot			= 0;
	m_yRot			= 0;
	m_zRot			= 0;
	m_translateX	= 0;
	m_translateY	= 0;
	m_depthImg		= false;
	m_applyMatrix	= false;

	glDraw			();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check whether the image is 3D or not
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief check whether the image is 3D or not
//! \return	return true if it's depth image, otherwise, return false
bool OpenGLWidget::isDepthImg()
{
	return m_depthImg;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return a copy of Vertex
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief return a copy of Vertex
//! \return		a copy of the struct Vertex
Vertex* OpenGLWidget::getVertex()
{
	Vertex *r	= new Vertex [17000];

	r[0].location[0]	= m_vertices[0].location[0];
	for (int i = 1; i < m_vertices[0].location[0]-1; i++)
	{
		r[i].location[0]	= m_vertices[i].location[0];
		r[i].location[1]	= m_vertices[i].location[1];
		r[i].location[2]	= m_vertices[i].location[2];

		r[i].colors[0]		= m_vertices[i].colors[0];
		r[i].colors[1]		= m_vertices[i].colors[1];
		r[i].colors[2]		= m_vertices[i].colors[2];
	}

	return r;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Apply transformation matrix
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief apply transformation matrix
//! \details apply the given matrix to 2nd point cloud
//! \param[in] *mat	transformation matrix
//! \param[in] *p	1st point cloud
//! \param[in] *q	2nd point cloud
// apply the given matrix to 2nd point cloud
void OpenGLWidget::transformation(double *mat, Vertex *p, Vertex *q)
{
	m_transformMat		= mat;
	m_applyMatrix		= true;

	glInit			();
	m_imageName		= QString();
	m_image			= QImage();
	m_depthImg		= true;

	m_ptCloud		= makeTransCloud(mat, p, q);

	// initial position
	m_yRot			= -90;
	m_translateX		= -8;
	m_translateY		= -3;
	glDraw();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Render transformed point cloud
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief render transformed point cloud
//! \param[in] *p	1st point cloud
//! \param[in] *q	2nd point cloud; transformed
void OpenGLWidget::drawCloud(Vertex *p, Vertex *q)
{
	m_applyMatrix		= false;
	glInit			();
	m_imageName		= QString();
	m_image			= QImage();
	m_depthImg		= true;

	m_ptCloud		= makeCloud(p, q);

	// initial position
	m_yRot			= -90;
	m_translateX		= -8;
	m_translateY		= -3;
	glDraw();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// stores the name of the image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief stores the name of the image
//! \param[in] fileName	file name to be stored
//! \param[in] image	image to be stored
void OpenGLWidget::storeImage(QString fileName, QImage image)
{
	glInit			();
	m_imageName		= fileName;
	m_image			= QImage(image);
	m_depthImg		= false;

	glDraw			();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// open new depth file and display it
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief store depth image
//! \param[in] fileName	file name to be stored
//! \param[in] path	read vertices from file
void OpenGLWidget::storeDepth(QString fileName, string path)
{
	glInit			();
	m_imageName		= fileName;
	m_image			= QImage();
	m_depthImg		= true;
	m_applyMatrix	= false;

	ifstream 		m_fileIn;
	m_fileIn.open(path.c_str());

	if (!m_fileIn.is_open())
		return;

	int index		= 1;
	string iLine;

	while (!m_fileIn.eof())
	{
		getline(m_fileIn, iLine);
		istringstream sstrm(iLine);
		sstrm >> m_vertices[index].location[0];
		sstrm >> m_vertices[index].location[1];
		sstrm >> m_vertices[index].location[2];
		sstrm >> m_vertices[index].colors[0];
		sstrm >> m_vertices[index].colors[1];
		sstrm >> m_vertices[index].colors[2];
		index++;
	}

	m_vertices[0].location[0]	= (double)(index-1);

	m_ptCloud	= makeCloud();

	// initial position
	m_yRot		= -90;
	m_translateX= -8;
	m_translateY= -3;
	glDraw();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// makes the image a texture
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief makes the image texture
void OpenGLWidget::createTexture()
{
	if(!m_imageName.isNull())
	{
		m_imageTexture	= bindTexture(m_image, GL_TEXTURE_2D);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// displays the image mapped to the rectangle
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief displays the image mapped to the rectangle
void OpenGLWidget::loadImage()
{
	glBindTexture	(GL_TEXTURE_2D, m_imageTexture);

	glPushMatrix	();

	glColor3f	(1.0, 1.0, 1.0);
	glPolygonMode	(GL_FRONT, GL_FILL);

	glBegin		(GL_QUADS);
	glTexCoord2f	(0, 0);		glVertex3i(-10, -10, 0);
	glTexCoord2f	(1, 0);		glVertex3i(10, -10, 0);
	glTexCoord2f	(1, 1);		glVertex3i(10, 10, 0);
	glTexCoord2i	(0, 1);		glVertex3i(-10, 10, 0);
	glEnd		();

	glPopMatrix	();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mouse press event
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief handles mouse press events
//! \param[in, out] e 	mouse event
void OpenGLWidget::mousePressEvent(QMouseEvent *e)
{
	if (m_image.isNull() && !m_depthImg)
		return;

	m_lastPt	= e->pos();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mouse move event
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief handles mouse move events
//! \param[in, out] e 	mouse event
//! \details one degree turn for every 8 pixels displaced
// one degree turn for every 8 pixels displaced
void OpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (m_image.isNull() && !m_depthImg)
		return;

	QPoint temp 	= mapTo(this, e->pos());
	if(temp.x() > width() || temp.x() < 0 || temp.y() > height() || temp.y() < 0)
	{	// if the mouse drag onto another frame (exceed current frame size), initiate dragging
		if(!m_image.isNull())
		{
			emit 	dragging();
			return;
		}
	}

	// calculate displacement based on last clicked point.
	int dx		= e->x() - m_lastPt.x();
	int dy		= e->y() - m_lastPt.y();

	if (e->buttons() & Qt::LeftButton)
	{
		setXRotation(m_xRot - dy);
		setYRotation(m_yRot - dx);
	}
	else if (e->buttons() & Qt::RightButton)
	{
		setXRotation(m_xRot - dy);
		setZRotation(m_zRot + dx);
	}
	m_lastPt	= e->pos();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Key Press
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief handles key press events
//! \details receive key events for zomming
//! \param[in, out] e	key event
// receive key events for zomming
void OpenGLWidget::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Up)
		m_translateY++;
	else if (e->key() == Qt::Key_Down)
		m_translateY--;
	else if (e->key() == Qt::Key_Left)
		m_translateX--;
	else if (e->key() == Qt::Key_Right)
		m_translateX++;
	else if(e->matches(QKeySequence::ZoomIn))
	{
		m_scale += 0.1f;
		if (m_scale > 3.0f)
			m_scale = 3.0f;
		emit zoomFactor(m_scale);
	}
	else if(e->matches(QKeySequence::ZoomOut))
	{
		m_scale -= 0.1f;
		if (m_scale < 0.1f)
			m_scale = 0.1f;
		emit zoomFactor(m_scale);
	}

	update();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// angle normalization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief normalize the rotation angle
//! \param[in, out] angle	pointer to the angle to be normalized
//! \details because the speed is one degree turn for every 8 pixels, the degree range is from 0 to 360*8
// because the speed is one degree turn for every 8 pixels, the degree range is from 0 to 360*8
void OpenGLWidget::normalizeAngle(int *angle)
{
	while (*angle < 0)
		*angle += 360 * 8;
	while (*angle > 360 * 8)
		*angle -= 360 * 8;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// set rotation angle for x-axis
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief  set rotation angle for x-axis
//! \param[in] angle	degree of rotation for x-axis
void OpenGLWidget::setXRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != m_xRot)
	{
		m_xRot = angle;
		update();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// set rotation angle for y-axis
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief  set rotation angle for y-axis
//! \param[in] angle	degree of rotation for y-axis
void OpenGLWidget::setYRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != m_yRot)
	{
		m_yRot = angle;
		update();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// set rotation angle for z-axis
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief  set rotation angle for z-axis
//! \param[in] angle	degree of rotation for z-axis
void OpenGLWidget::setZRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != m_zRot)
	{
		m_zRot = angle;
		update();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// make cloud list
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief make cloud list
//! \return	opengl list containing the vertices
GLuint OpenGLWidget::makeCloud()
{
	glPushMatrix	();
	glClear			(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint list		= glGenLists(1);
	glNewList		(list, GL_COMPILE);

	int size 		= (int)m_vertices[0].location[0];

	glBegin			(GL_POINTS);
	for (int i = 1; i < size-1; i++)
	{
		if(m_vertices[i].location[2] != 0)
		{
			glColor3f((float)m_vertices[i].colors[0]/255.0, (float)m_vertices[i].colors[1]/255.0, (float)m_vertices[i].colors[2]/255.0);
			glVertex3f(m_vertices[i].location[0], -m_vertices[i].location[1], -m_vertices[i].location[2]);
		}
	}
	glEnd			();

	glEndList		();
	glPopMatrix		();
	return list;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// make transformed cloud list based on inputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief make transformed cloud list based on inputs
//! \details 2nd point cloud (q) is transformed already and rendering it on top of 1st point cloud
//! \param[in] *p	1st point cloud
//! \param[in] *q	2nd point cloud
//! \return	opengl list containing the vertices
// 2nd point cloud (q) is transformed already and rendering it on top of 1st point cloud
GLuint OpenGLWidget::makeCloud(Vertex *p, Vertex *q)
{
	glPushMatrix	();
	glClear			(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint list		= glGenLists(1);
	glNewList		(list, GL_COMPILE);

	int size 		= (int)p[0].location[0];
	glBegin			(GL_POINTS);
	for (int i = 1; i < size-1; i++)
	{
		if(p[i].location[2] != 0)
		{
			glColor3f((float)p[i].colors[0]/255.0, (float)p[i].colors[1]/255.0, (float)p[i].colors[2]/255.0);
			glVertex3f(p[i].location[0], -p[i].location[1], -p[i].location[2]);
		}
	}

	size 		= (int)q[0].location[0];
	for (int i = 1; i < size-1; i++)
	{
		if(q[i].location[2] != 0)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(q[i].location[0], -q[i].location[1], -q[i].location[2]);
		}
	}

	glEnd			();

	glEndList		();
	glPopMatrix		();
	return list;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// make cloud list based on inputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! \brief make cloud list based on inputs
//! \details apply the transformation to the 2nd point cloud
//! \param[in] *mat	pointer to transformation matrix
//! \param[in] *p	1st point cloud
//! \param[in] *q	2nd point cloud
//! \return	opengl list containing the vertices
// apply the transformation to the 2nd point cloud
GLuint OpenGLWidget::makeTransCloud(double *mat, Vertex *p, Vertex *q)
{
	glPushMatrix	();
	glClear			(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint list		= glGenLists(1);
	glNewList		(list, GL_COMPILE);

	int size 		= (int)p[0].location[0];
	glBegin			(GL_POINTS);
	for (int i = 1; i < size-1; i++)
	{
		if(p[i].location[2] != 0)
		{
			glColor3f((float)p[i].colors[0]/255.0, (float)p[i].colors[1]/255.0, (float)p[i].colors[2]/255.0);
			glVertex3f(p[i].location[0], -p[i].location[1], -p[i].location[2]);
		}
	}

	glLoadMatrixd	(mat);
	size 		= (int)q[0].location[0];
	for (int i = 1; i < size-1; i++)
	{
		if(q[i].location[2] != 0)
		{
			glColor3f((float)q[i].colors[0]/255.0, (float)q[i].colors[1]/255.0, (float)q[i].colors[2]/255.0);
			glVertex3f(q[i].location[0], -q[i].location[1], -q[i].location[2]);
		}
	}

	glEnd			();

	glEndList		();
	glPopMatrix		();
	return list;
}
