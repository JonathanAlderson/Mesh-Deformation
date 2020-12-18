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

#include "MasterWidget.h"

MasterWidget::MasterWidget(char *filename, QWidget *parent)
{
    renderWidget = new RenderWidget(filename, this);


    // GUI Layout

    // Save/Load
    QGroupBox *saveLoadGroup = new QGroupBox(tr("Save/Load"));
    QPushButton *loadButton = new QPushButton("Load", this);
    QPushButton *saveButton = new QPushButton("Save", this);
    QVBoxLayout *saveLoadLayout = new QVBoxLayout;
    saveLoadLayout->addWidget(saveButton);
    saveLoadLayout->addWidget(loadButton);
    saveLoadGroup->setLayout(saveLoadLayout);

    // Interpolation Mode
    QGroupBox *interpolationGroup = new QGroupBox(tr("Interpolation Mode"));
    QRadioButton *bilinearRadio = new QRadioButton("Bilinear", this);
    QRadioButton *barycentricRadio = new QRadioButton("Barycentric", this);
    QRadioButton *trilinearRadio = new QRadioButton("Trilinear", this);
    barycentricRadio->setChecked(true);
    QVBoxLayout *interpolationLayout = new QVBoxLayout;
    interpolationLayout->addWidget(bilinearRadio);
    interpolationLayout->addWidget(barycentricRadio);
    interpolationLayout->addWidget(trilinearRadio);
    interpolationGroup->setLayout(interpolationLayout);

    // Settings
    QGroupBox *settingsGroup = new QGroupBox(tr("Settings"));
    QLabel *gridSizeLabel = new QLabel("Grid Size", this);
    QSpinBox *gridSizeSpinBox = new QSpinBox;
    gridSizeSpinBox->setRange(2, 100);
    gridSizeSpinBox->setSingleStep(1);
    gridSizeSpinBox->setValue(3);
    QLabel *gridScaleLabel = new QLabel("Grid Scale", this);
    QSlider *gridScaleSlider = new QSlider(Qt::Horizontal, 0);
    gridScaleSlider->setMaximum(200);
    gridScaleSlider->setMinimum(100);
    gridScaleSlider->setValue(150);
    QLabel *attenuationLabel = new QLabel("Attenuation", this);
    QSlider *attenuationSlider = new QSlider(Qt::Horizontal, 0);
    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(gridSizeLabel);
    settingsLayout->addWidget(gridSizeSpinBox);
    settingsLayout->addWidget(gridScaleLabel);
    settingsLayout->addWidget(gridScaleSlider);
    settingsLayout->addWidget(attenuationLabel);
    settingsLayout->addWidget(attenuationSlider);
    settingsGroup->setLayout(settingsLayout);

    // all ui group
    QGroupBox *allUI = new QGroupBox("Options");
    QVBoxLayout *allUILayout = new QVBoxLayout;
    allUILayout->addWidget(saveLoadGroup);
    allUILayout->addWidget(interpolationGroup);
    allUILayout->addWidget(settingsGroup);
    allUI->setLayout(allUILayout);
    allUI->setMaximumWidth(300);

    // render box group
    QGroupBox *renderGroup = new QGroupBox("Render");
    QVBoxLayout *renderGroupLayout = new QVBoxLayout;
    renderGroupLayout->addWidget(renderWidget);
    renderGroup->setLayout(renderGroupLayout);
    renderGroup->setMinimumWidth(600);
    renderGroup->setMinimumHeight(600);

    // Master layout
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(renderGroup, 0, 1);
    mainLayout->addWidget(allUI, 0, 0);
    setLayout(mainLayout);

    attenuationSlider->setValue(0);
    setWindowTitle(tr("Mesh Deformation Editor"));

    // Connecting
    connect(attenuationSlider, SIGNAL(valueChanged(int)), renderWidget, SLOT(setAttenuation(int)));
    connect(gridScaleSlider, SIGNAL(valueChanged(int)), renderWidget, SLOT(setGridScale(int)));
    connect(gridSizeSpinBox, SIGNAL(valueChanged(int)), renderWidget, SLOT(setGridSize(int)));

    connect(loadButton, SIGNAL(pressed()), renderWidget, SLOT(loadButtonPressed()));
    connect(saveButton, SIGNAL(pressed()), renderWidget, SLOT(saveButtonPressed()));

    connect(bilinearRadio, SIGNAL(clicked()), renderWidget, SLOT(bilinearRadioClicked()));
    connect(barycentricRadio, SIGNAL(clicked()), renderWidget, SLOT(barycentricRadioClicked()));
    connect(trilinearRadio, SIGNAL(clicked()), renderWidget, SLOT(trilinearRadioClicked()));

    //
}

QSlider *MasterWidget::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void MasterWidget::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
         QCoreApplication::quit();
    }
    else
        QWidget::keyPressEvent(event);
}
