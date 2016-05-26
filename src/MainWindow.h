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
#include <QUndoStack>
#include <QTimer>

#include "AboutDialog.h"
#include "ExportImageDialog.h"
#include "GLWidget.h"
#include "LayerListWidget.h"
#include "MapFileIO.h"
#include "PlaybackWidget.h"
#include "PreferencesDialog.h"
#include "Settings.h"
#include "TrackFileReader.h"
#include "TrackLayer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    
    virtual ~MainWindow();
    
    void loadTrackFile(const QString &trackFilePath);
    
    bool loadMapFile(const QString &path);
    
    bool addLayer(Layer *layer);
    
    LayerPtr getLayerPtr(LayerId layerId);
    
    LayerPtrList getLayers() const;
    
    void removeLayers(const QList<LayerId> &layerIds);
    
    void clearMap();
    
    void setLayerName(LayerId layerId, const QString &name);
    
    void setLayerSport(LayerId layerId, const QString &sport);
    
    bool saveMapFile(const QString &path);
    
    bool confirmAbandonMap();
    
    QString getNetworkCacheDir() const;
    
    void saveSettings();
    
    void restoreSettings();
    
    void applyTrackStyleRule(const TrackStyleRules &rules, TrackLayer *layer);
    
    void applyTrackStyleRules(const TrackStyleRules &rules);
    
    /* Deferred update restarts a timer that will update the map after a short
     interval of time. This is used to prevent unnecessary redraws from 
     happening when loading thousands of new layers.
     */
    void deferredUpdate();
    
public slots:
    bool slotSaveMapFile();
    
    bool slotSaveMapFileAs();
    
    bool slotOpenMapFile();
    
    bool slotOpenMapFile(const QString &path);
    
    void slotNewMap();
    
    void slotAddLayers();
    
    void slotAddLayers(const QStringList &paths);
    
    void slotRemoveLayers(const QList<LayerId> &layerIds);
    
    void slotPlaybackRateChanged(double newRate);
    
    void slotTimeChanged(double mapSeconds);
    
    void onTimeSliderDrag(int seconds);
    
    void slotLayerAdded(LayerId);
    
    void slotFrameLayers(const QList<LayerId> layerIds);
    
    void slotLayerSelectionChanged(const QList<LayerId> layerIds);
    
    void slotLayerVisibilityChanged(LayerId layerId, bool visible);
    
    /* Slot for when the user edits a layer's name */
    void slotUserChangedLayerName(LayerId layerId, const QString &newName);
    
    /* Slot for when the user edits the layer's sport field */
    void slotUserChangedLayerSport(LayerId layerId, const QString &newSport);
    
    void slotShowAboutDialog();
    
    void slotShowPreferencesDialog();
    
    void slotShowPlaybackWidget();
    
    void slotShowLayerListWidget();
    
    void slotShowMapWindow();
    
    void slotTrackFileLoaded(const QString &path, QList<Track*> *tracks);
    
    void slotTrackFileLoadError(const QString &path, const QString &what);
    
    void slotLockViewToSelectedLayer();
    
    void slotFrameSelectedLayers();
    
    void slotOpenRecentMapFile(QAction *mapAction);
    
    void slotAddRecentLayer(QAction *layerAction);
    
    void slotSetStartingViewArea();
    
    void slotAboutToQuit();
    
    void slotUpdate();
    
protected:
    
    enum {
        _numRecentFiles = 5
    };
    
    void _layoutPlaybackControls(QWidget*);
    
    void _setupShortcuts();
    
    void _loadBaseMap();
    
    void _addPathToRecentMenu(QMenu *theMenu, const QString &path);
    
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
            *_lockViewToLayerAction, *_showPreferencesDialogAction,
            *_clearRecentLayersMenuAction, *_clearRecentMapsMenuAction,
            *_setStartingViewAreaAction, *_showExportImagesDialogAction,
            *_undoAction, *_redoAction;
    QShortcut *_playPauseShortcut, *_addLayerShortcut, *_saveMapShortcut,
              *_openMapShortcut, *_newMapShortcut, *_playShortcut,
              *_playReverseShortcut, *_pauseShortcut;
    QMenu *_recentMapsMenu, *_recentLayersMenu;
    QNetworkAccessManager *_networkAccessManager;
    QNetworkDiskCache *_diskCache;
    QStringList _trackFiles;
    TrackFileReader *_trackFileReader;
    QString _lastLayerPathAdded;
    QList<LayerId> _layersToFrame;
    int _numPendingLayers;
    AboutDialog *_aboutDialog;
    Settings *_settings;
    PreferencesDialog *_preferencesDialog;
    ExportImageDialog *_exportImageDialog;
    QUndoStack *_undoStack;
    QTimer *_deferredUpdateTimer;
};

#endif
