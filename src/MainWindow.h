#ifndef RUNPARTICLES_MAINWINDOW_H
#define RUNPARTICLES_MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLineEdit>
#include <QMenuBar>
#include <QProgressDialog>
#include <QShortcut>
#include <QSlider>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

#include "AboutDialog.h"
#include "GLWidget.h"
#include "LayerListWidget.h"
#include "MapFileIO.h"
#include "PlaybackWidget.h"
#include "Settings.h"
#include "TrackFileReader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();
    
    void loadTrackFile(const QString &trackFilePath);
    
    bool loadMapFile(const QString &path);
    
    void clearMap();
    
    bool saveMapFile(const QString &path);
    
    bool confirmAbandonMap();
    
    QString getNetworkCacheDir() const;
    
    void closeEvent(QCloseEvent *event);
    
    void saveSettings();
    
    void restoreSettings();
    
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
    
    void slotShowAboutDialog();
    
    void slotShowPlaybackWidget();
    
    void slotShowLayerListWidget();
    
    void slotShowMapWindow();
    
    void slotTrackFileLoaded(const QString &path, QList<Track*> *tracks);
    
    void slotTrackFileLoadError(const QString &path, const QString &what);
    
    void slotLockViewToSelectedLayer();
    
    void slotFrameSelectedLayers();
    
protected:
    
    void _layoutPlaybackControls(QWidget*);
    
    void _setupShortcuts();
    
    void _loadBaseMap();
    
    void _showWidget(QWidget *widget);
    
    MapFileIO *_fileIO;
    
    QMenuBar *_menuBar;
    GLWidget *_glWidget;
    PlaybackWidget *_playbackWidget;
    LayerListWidget *_layerListWidget;
    QAction *_newMapAction, *_openMapFileAction, *_saveMapFileAction,
            *_saveAsMapFileAction, *_addLayerAction, *_forwardAction,
            *_backAction, *_rewindAction, *_pauseAction,
            *_playCtrlWindowAction, *_layerListWindowAction, *_mapWindowAction,
            *_showAboutDialogAction, *_frameSelectedLayersAction,
            *_lockViewToLayerAction;
    QShortcut *_playPauseShortcut, *_addLayerShortcut, *_saveMapShortcut,
              *_openMapShortcut, *_newMapShortcut, *_playShortcut,
              *_playReverseShortcut, *_pauseShortcut;
    QNetworkAccessManager *_networkAccessManager;
    QNetworkDiskCache *_diskCache;
    QStringList _trackFiles;
    TrackFileReader *_trackFileReader;
    QString _lastLayerPathAdded;
    QList<LayerId> _layersToFrame;
    int _numPendingLayers;
    AboutDialog *_aboutDialog;
    Settings *_settings;
};

#endif
