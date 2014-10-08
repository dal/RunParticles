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
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

#include "GLWidget.h"
#include "LayerListWidget.h"
#include "MapFileIO.h"
#include "TrackFileReader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(GLWidget *glWidget, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();
    
    void loadTrackFile(const QString &trackFilePath);
    
    bool loadMapFile(const QString &path);
    
    void clearMap();
    
    bool saveMapFile(const QString &path);
    
    bool confirmAbandonMap();
    
    QString getNetworkCacheDir() const;
    
public slots:
    bool slotSaveMapFile();
    
    bool slotSaveMapFileAs();
    
    bool slotOpenMapFile();
    
    void slotNewMap();
    
    void slotAddLayer();
    
    void slotPlaybackRateChanged(const QString &newRate);
    
    void slotTimeChanged(double mapSeconds);
    
    void onTimeSliderDrag(int seconds);
    
    void slotLayerAdded(LayerId);
    
    void slotFrameLayers(const QList<LayerId> layerIds);
    
    void slotLayerSelectionChanged(const QList<LayerId> layerIds);
    
    void slotLayerVisibilityChanged(LayerId layerId, bool visible);
    
    void slotShowPlaybackWidget();
    
    void slotShowLayerListWidget();
    
    void slotShowMapWindow();
    
    void slotTrackFileLoaded(const QString &path, QList<Track*> *tracks);
    
    void slotTrackFileLoadError(const QString &path, const QString &what);
    
    void slotMaximizeGLWidget();
    
    void slotPlay();
    
signals:
    void signalDoneReading();
    
protected:
    void _layoutPlaybackControls(QWidget*);
    
    void _setupShortcuts();
    
    void _loadBaseMap();
    
    void _showWidget(QWidget *widget);
    
    MapFileIO *_fileIO;
    
    QMenuBar *_menuBar;
    GLWidget *_glWidget;
    LayerListWidget *_layerListWidget;
    QPushButton *_rewindButton, *_forwardButton, *_pauseButton, *_backButton;
    QSlider *_slider;
    QLineEdit *_currentTimeLineEdit;
    QComboBox *_playSpeedCombo;
    QAction *_newMapAction, *_openMapFileAction, *_saveMapFileAction,
            *_saveAsMapFileAction, *_addLayerAction, *_forwardAction,
            *_backAction, *_rewindAction, *_pauseAction,
            *_playCtrlWindowAction, *_layerListWindowAction, *_mapWindowAction;
    QShortcut *_playPauseShortcut, *_addLayerShortcut, *_saveMapShortcut,
              *_openMapShortcut, *_newMapShortcut, *_playShortcut,
              *_playReverseShortcut, *_pauseShortcut;
    QNetworkAccessManager *_networkAccessManager;
    QNetworkDiskCache *_diskCache;
    QStringList _trackFiles;
    TrackFileReader *_trackFileReader;
};

#endif
