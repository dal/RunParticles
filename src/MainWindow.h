#ifndef RUNPARTICLES_MAINWINDOW_H
#define RUNPARTICLES_MAINWINDOW_H

#include <QMainWindow>
#include "GLWidget.h"
#include <QAction>
#include <QMenuBar>
#include <QPushButton>
#include <QComboBox>

class MainWindow : public QMainWindow
{
public:
    MainWindow(GLWidget *glWidget, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();
    
protected:
    QMenuBar *_menuBar;
    GLWidget *_glWidget;
    QPushButton *_forwardButton, *_backButton;
    QComboBox *_playSpeedCombo;
    QAction *_openLayerAction;
};

#endif
