///////////////////////////////////////////////////
//
//	Jonathan Alderson
//	October, 2020
//
//	------------------------
//	MasterWidget.h
//	------------------------
//
//	The main widget that houses the GUI and the OpenGL
//
///////////////////////////////////////////////////

#ifndef _MASTER_WIDGET_H
#define _MASTER_WIDGET_H

#include <QGLWidget>
#include "RenderWidget.h"
#include <QSlider>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QCoreApplication>

class MasterWidget : public QWidget
{
    Q_OBJECT

public:
    MasterWidget(char *filename, QWidget *parent);

    void keyPressEvent(QKeyEvent* event); // so we can escape to quit

private:
    QSlider *createSlider();

    QGLWidget *renderWidget;

    QSlider *attenuationSlider;
    QPushButton *loadButton;
    QPushButton *saveButton;
    QRadioButton *bilinearRadio;
    QRadioButton *barycentricRadio;
    QRadioButton *trilinearRadio;

};

#endif
