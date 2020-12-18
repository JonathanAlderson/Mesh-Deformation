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

#ifndef _GEOMETRIC_WIDGET_H
#define _GEOMETRIC_WIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include "GeometricSurfaceFaceDS.h"
#include "Ball.h"
#include "MousePick.h"

class RenderWidget : public QGLWidget
	{ // class RenderWidget
	Q_OBJECT
	public:
	// the model - i.e. the surface
	GeometricSurfaceFaceDS *surface;

	// arcball for storing light rotation
	BallData lightBall;

	// arcball for storing object rotation
	BallData objectBall;

	// for resolving clicks on the screen to 3D coordinates
	MousePick *mousePicker;

	// translation in window x,y
	GLfloat translate_x, translate_y;
	GLfloat last_x, last_y;

	// which button was last pressed
	int whichButton;

	QString newFileName;



	// constructor
	RenderWidget(char *filename, QWidget *parent = 0);

	// destructor
	~RenderWidget();

	protected:
	// called when OpenGL context is set up
	void initializeGL();
	// called every time the widget is resized
	void resizeGL(int w, int h);
	// called every time the widget needs painting
	void paintGL();

	// mouse-handling
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

	// Resizing functions
	QSize minimumSizeHint();
	QSize sizeHint();

	// gui controls
	public slots:
    void setAttenuation(int x);
		void setGridScale(int x);
		void setGridSize(int x);
		void loadButtonPressed();
		void saveButtonPressed();
		void bilinearRadioClicked();
		void barycentricRadioClicked();
		void trilinearRadioClicked();

	signals:
	    void attenuationChanged(int x);
			void gridScaleChanged(int x);
			void gridSizeChanged(int x);


  private:
		int xRot;
		int attenuation;
		float gridScale;
		int gridX;
		int gridY;
		int interpMode;

	}; // class RenderWidget

#endif
