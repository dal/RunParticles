#include "MainWindow.h"

#include <QGridLayout>
#include <QMenu>
#include <QWidget>

MainWindow::MainWindow(GLWidget *glWidget,
                       QWidget * parent,
                       Qt::WindowFlags flags) :
    QMainWindow::QMainWindow(parent, flags),
    _menuBar(new QMenuBar(0)),
    _glWidget(glWidget)
{
    QWidget *ctr = new QWidget(this);
    setCentralWidget(ctr);
    QGridLayout *_layout = new QGridLayout(ctr);
    _backButton = new QPushButton("<", ctr);
    _layout->addWidget(_backButton, 0, 0);
    _forwardButton = new QPushButton(">", ctr);
    _layout->addWidget(_forwardButton, 0, 1);
    _playSpeedCombo = new QComboBox(ctr);
    _playSpeedCombo->addItem("1x");
    _playSpeedCombo->addItem("2x");
    _playSpeedCombo->addItem("4x");
    _playSpeedCombo->addItem("8x");
    _playSpeedCombo->addItem("16x");
    _playSpeedCombo->addItem("32x");
    _playSpeedCombo->addItem("64x");
    _layout->addWidget(_playSpeedCombo, 0, 2);
    QMenu *_fileMenu = _menuBar->addMenu("File");
    _openLayerAction = new QAction("Open Layer. . .", ctr);
    _fileMenu->addAction(_openLayerAction);
}

MainWindow::~MainWindow()
{
    // empty
}

