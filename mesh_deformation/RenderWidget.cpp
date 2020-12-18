///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	RenderWidget.h
//	------------------------
//
//	The main widget that shows the geometry
//
///////////////////////////////////////////////////

#include <math.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "RenderWidget.h"

static GLfloat light_position[] = {0.0, 0.0, 1.0, 0.0};

// constructor
RenderWidget::RenderWidget(char *filename, QWidget *parent)
	: QGLWidget(parent)
	{ // constructor

		// set default values
		whichButton = -1;
		translate_x = translate_y = 0.0;
		xRot = 0;
		attenuation = 0;
		gridScale = 1.5;
		gridX = 3;
		gridY = 3;
		interpMode = 1;

	// store pointer to the model
	surface = new GeometricSurfaceFaceDS();
	surface->ReadFileTriangleSoup(filename);
	surface->GenerateCage(gridScale, gridX, gridY, interpMode);
	surface->FindInterpolations();

	// initialise the mouse clicker
	mousePicker = new MousePick(&(surface->cagePoints));

	// initialise arcballs to 80% of the widget's size
	Ball_Init(&lightBall);		Ball_Place(&lightBall, qOne, 0.80);
	Ball_Init(&objectBall);		Ball_Place(&objectBall, qOne, 0.80);

	} // constructor

// destructor
RenderWidget::~RenderWidget()
	{ // destructor
	// nothing yet
	} // destructor

// called when OpenGL context is set up
void RenderWidget::initializeGL()
	{ // RenderWidget::initializeGL()
	// enable Z-buffering
	glEnable(GL_DEPTH_TEST);

	// set lighting parameters
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// background is pink
	glClearColor(.0, 0.7, 0.7, 1.0);
	} // RenderWidget::initializeGL()

// called every time the widget is resized
void RenderWidget::resizeGL(int w, int h)
	{ // RenderWidget::resizeGL()
	// reset the viewport
	glViewport(0, 0, w, h);

	// set projection matrix to be glOrtho based on zoom & window size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// retrieve the scale factor
	float size = surface->boundingSphereSize;

	std::cout << "Size: " << size << '\n';

	// compute the aspect ratio of the widget
	float aspectRatio = (float) w / (float) h;

	// depending on aspect ratio, set to accomodate a sphere of radius = diagonal without clipping
	if (aspectRatio > 1.0)
		glOrtho(-aspectRatio * size, aspectRatio * size, -size, size, -size, size);
	else
		glOrtho(-size, size, -size/aspectRatio, size/aspectRatio, -size, size);

	} // RenderWidget::resizeGL()

// called every time the widget needs painting
void RenderWidget::paintGL()
	{ // RenderWidget::paintGL()
	// clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set lighting on
	glEnable(GL_LIGHTING);

	// set model view matrix based on stored translation, rotation &c.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set light position first
	// retrieve rotation from arcball & apply
	GLfloat mNow[16];
	Ball_Value(&lightBall, mNow);
	glMultMatrixf(mNow);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// apply translation for interface control
	glLoadIdentity();
	glTranslatef(translate_x, translate_y, 0.0);


	// apply rotation matrix from arcball
	Ball_Value(&objectBall, mNow);
	glMultMatrixf(mNow);

	// now draw the surface
	//surface->RenderInterpolate();

	// finally draw the cage
	surface->RenderCage();

	} // RenderWidget::paintGL()

// mouse-handling
void RenderWidget::mousePressEvent(QMouseEvent *event)
	{ // RenderWidget::mousePressEvent()
	// store the button for future reference
	whichButton = event->button();

	// find the minimum of height & width
	float size = (width() > height()) ? height() : width();

	// convert to the ArcBall's vector type
	HVect vNow;

	// scale both coordinates from that
	vNow.x = (2.0 * event->x() - size) / size;
	vNow.y = (size - 2.0 * event->y() ) / size;

	// Perform Mouse Picking
	if(mousePicker->click(vNow.x, vNow.y) == -1) // if we arn't dragging a grid
	{
		// now either translate or rotate object or light
		switch(whichButton)
		{ // button switch
			case Qt::RightButton:
			// save the last x, y
			last_x = vNow.x; last_y = vNow.y;
			// and update
			updateGL();
			break;
			case Qt::MiddleButton:
			// pass the point to the arcball code
			Ball_Mouse(&lightBall, vNow);
			// start dragging
			Ball_BeginDrag(&lightBall);
			// update the widget
			updateGL();
			break;
			case Qt::LeftButton:
			// pass the point to the arcball code
			Ball_Mouse(&objectBall, vNow);
			// start dragging
			Ball_BeginDrag(&objectBall);
			// update the widget
			updateGL();
			break;
		} // button switch
	}
	else
	{
	}

	} // RenderWidget::mousePressEvent()

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
	{ // RenderWidget::mouseMoveEvent()
	// find the minimum of height & width
	float size = (width() > height()) ? height() : width();

	// convert to the ArcBall's vector type
	HVect vNow;

	// scale both coordinates from that
	vNow.x = (2.0 * event->x() - size) / size;
	vNow.y = (size - 2.0 * event->y() ) / size;

	// now either translate or rotate object or light
	if(mousePicker->dragging == false)
	{
		switch(whichButton)
		{ // button switch
			case Qt::RightButton:
			// subtract the translation
			translate_x += vNow.x - last_x;
			translate_y += vNow.y - last_y;
			last_x = vNow.x;
			last_y = vNow.y;
			// update the widget
			updateGL();
			break;
			case Qt::MiddleButton:
			// pass it to the arcball code
			Ball_Mouse(&lightBall, vNow);
			// start dragging
			Ball_Update(&lightBall);
			// update the widget
			updateGL();
			break;
			case Qt::LeftButton:
			// pass it to the arcball code
			Ball_Mouse(&objectBall, vNow);
			// start dragging
			Ball_Update(&objectBall);
			// update the widget
			updateGL();
			break;
		} // button switch
	}
	// Adjust one of the grid points
	else
	{
		surface->updateCagePoint(mousePicker->closest, mousePicker->drag(vNow.x, vNow.y));
		paintGL();
		updateGL();
	}
	} // RenderWidget::mouseMoveEvent()

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
	{ // RenderWidget::mouseReleaseEvent()
	// now either translate or rotate object or light
	mousePicker->dragging = false; // stop from dragging

	switch(whichButton)
		{ // button switch
		case Qt::RightButton:
			// just update
			updateGL();
			break;
		case Qt::MiddleButton:
			// end the drag
			Ball_EndDrag(&lightBall);
			// update the widget
			updateGL();
			break;
		case Qt::LeftButton:
			// end the drag
			Ball_EndDrag(&objectBall);
			// update the widget
			updateGL();
			break;
		} // button switch
	} // RenderWidget::mouseReleaseEvent()

	QSize RenderWidget::minimumSizeHint()
	{
		return QSize(50, 50);
	}


	QSize RenderWidget::sizeHint()
	{
		return QSize(600, 600);
	}


// GUI FUNCTIONS
void RenderWidget::setAttenuation(int x)
{
	if (x != attenuation) {
		  std::cout << "Attenuation Changed: " << x << '\n';
			attenuation = x;
			emit attenuationChanged(x);
	}
}

void RenderWidget::setGridScale(int x)
{
	float y = (float)x / 100.;
	if (y != gridScale) {
		  std::cout << "Grid Scale Changed: " << y << '\n';
			gridScale = y;
			emit gridScaleChanged(x);
			surface->GenerateCage(y, -1, -1, -1);
			surface->FindInterpolations();
			updateGL();
			paintGL();
	}
}

void RenderWidget::setGridSize(int x)
{
	if (x != gridX) {
		  std::cout << "Grid Size Changed: " << x << " " << x << '\n';
			gridX = x;
			gridY = x;
			emit gridSizeChanged(x);
			surface->GenerateCage(-1, x, x, -1);
			surface->FindInterpolations();
			updateGL();
			paintGL();
	}
}


void RenderWidget::loadButtonPressed()
{
	newFileName = QFileDialog::getOpenFileName(this,
    tr("Open Model"), "../models", tr("Model Files (*.tri)"));
	std::cout << "Load button pressed" << '\n';
	std::cout << newFileName.toStdString() << '\n';

	surface->ReadFileTriangleSoup(newFileName.toLocal8Bit().data());
	surface->GenerateCage(-1, -1, -1, -1); // -1 so we don't change the value
	surface->FindInterpolations();
	resizeGL(600, 600); // resets the zoom
	paintGL();
}

void RenderWidget::saveButtonPressed()
{
	std::cout << "Save button pressed" << '\n';
}

void RenderWidget::bilinearRadioClicked()
{
	surface->GenerateCage(-1, -1, -1, 0);
	updateGL();
	paintGL();
}

void RenderWidget::barycentricRadioClicked()
{
	surface->GenerateCage(-1, -1, -1, 1);
	updateGL();
	paintGL();
}

void RenderWidget::trilinearRadioClicked()
{
	surface->GenerateCage(-1, -1, -1, 2);
	updateGL();
	paintGL();
}
