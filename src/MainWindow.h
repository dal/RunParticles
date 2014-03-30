#ifndef RUNPARTICLES_MAINWINDOW_H
#define RUNPARTICLES_MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLineEdit>
#include <QMenuBar>
#include <QPushButton>
#include <QComboBox>
#include <QShortcut>
#include <QSlider>

#include "GLWidget.h"
#include "LayerListWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(GLWidget *glWidget, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();
    
    void loadTrackFile(const QString &path);
    
protected:
    void _layout(QWidget*);
    
public slots:
    void slotLoadFile();
    
    void slotPlaybackRateChanged(const QString &newRate);
    
    void slotTimeChanged(double mapSeconds);
    
    void slotLayerAdded();
    
    void slotFrameLayers(const QList<LayerId> layerIds);
    
    void slotLayerSelectionChanged(const QList<LayerId> layerIds);
    
    void slotLayerVisibilityChanged(LayerId layerId, bool visible);
    
protected:
    void _loadBaseMap();
    
    QMenuBar *_menuBar;
    GLWidget *_glWidget;
    LayerListWidget *_layerListWidget;
    QPushButton *_rewindButton, *_forwardButton, *_pauseButton, *_backButton;
    QSlider *_slider;
    QLineEdit *_currentTimeLineEdit;
    QComboBox *_playSpeedCombo;
    QAction *_openLayerAction, *_forwardAction, *_backAction, *_rewindAction,
            *_pauseAction;
    QShortcut *_playPauseShortcut, *_openLayerShortcut;
};

#endif
