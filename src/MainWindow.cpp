#include "MainWindow.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QEventLoop>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

#include "MapFileIO.h"
#include "OsmLayer.h"
#include "TrackLayer.h"

MainWindow::MainWindow(QWidget * parent,
                       Qt::WindowFlags flags) :
    QMainWindow::QMainWindow(parent, flags),
    _fileIO(new MapFileIO(this)),
    _menuBar(new QMenuBar(0)),
    _glWidget(new GLWidget()),
    _playbackWidget(new PlaybackWidget()),
    _layerListWidget(new LayerListWidget()),
    _trackFileReader(new TrackFileReader(this)),
    _numPendingLayers(0),
    _aboutDialog(new AboutDialog(this)),
    _settings(new Settings(this))
{
    _networkAccessManager = Singleton<QNetworkAccessManager>::Instance();
    _networkAccessManager->setParent(this);
    _diskCache = new QNetworkDiskCache(this);
    QString cacheDir = getNetworkCacheDir();
    _diskCache->setCacheDirectory(cacheDir);
    _networkAccessManager->setCache(_diskCache);
    
    /* The trackFileReader reads tracks in the background */
    connect(_trackFileReader, &TrackFileReader::signalReady,
            this, &MainWindow::slotTrackFileLoaded);
    connect(_trackFileReader, &TrackFileReader::signalError,
            this, &MainWindow::slotTrackFileLoadError);
    connect(_trackFileReader, &TrackFileReader::signalUpdate,
            _playbackWidget, &PlaybackWidget::updateStatus);
    connect(_trackFileReader, &TrackFileReader::signalDone,
            _playbackWidget, &PlaybackWidget::hideProgress);
    
    // Placeholder hidden centralWidget to keep Qt happy
    QWidget *myCentralWidget = new QWidget();
    setCentralWidget(myCentralWidget);
    centralWidget()->hide();
    
    /* file menu */
    QMenu *_fileMenu = _menuBar->addMenu("File");
    _newMapAction = new QAction("&New Map", this);
    _openMapFileAction = new QAction("&Open Map. . .", this);
    _saveAsMapFileAction = new QAction("Save Map As. . .", this);
    _saveMapFileAction = new QAction("&Save Map", this);
    _addLayerAction = new QAction("&Add Track File. . .", this);
    _fileMenu->addAction(_newMapAction);
    _fileMenu->addAction(_openMapFileAction);
    _fileMenu->addAction(_saveMapFileAction);
    _fileMenu->addAction(_saveAsMapFileAction);
    _fileMenu->addAction(_addLayerAction);
    connect(_newMapAction, SIGNAL(triggered(bool)),
            this, SLOT(slotNewMap()));
    connect(_openMapFileAction, SIGNAL(triggered(bool)),
            this, SLOT(slotOpenMapFile()));
    connect(_saveMapFileAction, SIGNAL(triggered(bool)),
            this, SLOT(slotSaveMapFile()));
    connect(_saveAsMapFileAction, SIGNAL(triggered(bool)),
            this, SLOT(slotSaveMapFileAs()));
    connect(_addLayerAction, SIGNAL(triggered(bool)),
            this, SLOT(slotAddLayer()));
    
    /* View menu */
    QMenu *_viewMenu = _menuBar->addMenu("View");
    _frameSelectedLayersAction = new QAction("Frame selected layers", this);
    _lockViewToLayerAction = new QAction("Lock view to selected layer", this);
    _viewMenu->addAction(_frameSelectedLayersAction);
    _viewMenu->addAction(_lockViewToLayerAction);
    connect(_lockViewToLayerAction, &QAction::triggered,
            this, &MainWindow::slotLockViewToSelectedLayer);
    connect(_frameSelectedLayersAction, &QAction::triggered,
            this, &MainWindow::slotFrameSelectedLayers);
    
    /* Window menu */
    QMenu *_windowMenu = _menuBar->addMenu("Window");
    _playCtrlWindowAction = new QAction("Playback controls", this);
    _layerListWindowAction = new QAction("Layer list", this);
    _mapWindowAction = new QAction("Map", this);
    _windowMenu->addAction(_playCtrlWindowAction);
    _windowMenu->addAction(_layerListWindowAction);
    _windowMenu->addAction(_mapWindowAction);
    connect(_playCtrlWindowAction, SIGNAL(triggered()),
            SLOT(slotShowPlaybackWidget()));
    connect(_layerListWindowAction, SIGNAL(triggered()),
            SLOT(slotShowLayerListWidget()));
    connect(_mapWindowAction, SIGNAL(triggered()), SLOT(slotShowMapWindow()));
    
    /* Playback controls */
    _forwardAction = new QAction("Play", this);
    _backAction = new QAction("Reverse", this);
    _rewindAction = new QAction("Rewind", this);
    _pauseAction = new QAction("Pause", this);
    
    connect(_playbackWidget, SIGNAL(signalForward()),
            _forwardAction, SLOT(trigger()));
    connect(_playbackWidget, SIGNAL(signalBack()),
            _backAction, SLOT(trigger()));
    connect(_playbackWidget, SIGNAL(signalPause()),
            _pauseAction, SLOT(trigger()));
    connect(_playbackWidget, SIGNAL(signalRewind()),
            _rewindAction, SLOT(trigger()));
    connect(_playbackWidget, SIGNAL(signalPlaybackRateChanged(const QString&)),
            this, SLOT(slotPlaybackRateChanged(const QString&)));
    
    connect(_forwardAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotPlay()));
    connect(_backAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotReverse()));
    connect(_pauseAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotPause()));
    connect(_rewindAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotRewind()));
    
    connect(_playbackWidget, SIGNAL(signalTimeSliderChanged(int)),
            SLOT(onTimeSliderDrag(int)));
    
    connect(_glWidget, SIGNAL(signalTimeChanged(double)),
            this, SLOT(slotTimeChanged(double)));
    connect(_glWidget->getMap(), SIGNAL(signalLayerAdded(LayerId)),
            this, SLOT(slotLayerAdded(LayerId)));
    connect(_glWidget, SIGNAL(signalLayersSelected(QList<LayerId>)),
            _layerListWidget, SLOT(slotSetSelectedLayers(QList<LayerId>)));
    
    /* About menu */
    _showAboutDialogAction = new QAction("About RunParticles", this);
    _showAboutDialogAction->setMenuRole(QAction::AboutRole);
    connect(_showAboutDialogAction, &QAction::triggered,
            this, &MainWindow::slotShowAboutDialog);
    QMenu *aboutMenu = _menuBar->addMenu("About . . .");
    aboutMenu->addAction(_showAboutDialogAction);
    
    // Application keyboard shortcuts
    _setupShortcuts();
    
    // Connect the layer list signals
    connect(_layerListWidget, SIGNAL(signalFrameLayers(QList<LayerId>)),
            this, SLOT(slotFrameLayers(const QList<LayerId>)));
    connect(_layerListWidget,
            SIGNAL(signalLayerSelectionChanged(QList<LayerId>)),
            this, SLOT(slotLayerSelectionChanged(const QList<LayerId>)));
    connect(_layerListWidget,
            SIGNAL(signalLayerVisibilityChanged(LayerId, bool)),
            this, SLOT(slotLayerVisibilityChanged(LayerId, bool)));
    connect(_layerListWidget,
            SIGNAL(signalLockViewToLayer(LayerId)),
            _glWidget, SLOT(slotLockViewToLayer(LayerId)));
    
    slotTimeChanged(0);
    restoreSettings();
    _layerListWidget->show();
    _playbackWidget->show();
    _glWidget->show();
    
    _loadBaseMap();
}

MainWindow::~MainWindow()
{
    // empty
}

void
MainWindow::_setupShortcuts()
{
    // Application-wide shortcuts
    // toggle play
    _playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    _playPauseShortcut->setContext(Qt::ApplicationShortcut);
    connect(_playPauseShortcut, SIGNAL(activated()),
            _glWidget, SLOT(slotTogglePlayPause()));
    // play
    _playShortcut = new QShortcut(QKeySequence(Qt::Key_L), this);
    _playShortcut->setContext(Qt::ApplicationShortcut);
    connect(_playShortcut, SIGNAL(activated()),
            _glWidget, SLOT(slotPlay()));
    // play in reverse
    _playReverseShortcut = new QShortcut(QKeySequence(Qt::Key_J), this);
    _playReverseShortcut->setContext(Qt::ApplicationShortcut);
    connect(_playReverseShortcut, SIGNAL(activated()),
            _glWidget, SLOT(slotReverse()));
    // pause
    _pauseShortcut = new QShortcut(QKeySequence(Qt::Key_K), this);
    _pauseShortcut->setContext(Qt::ApplicationShortcut);
    connect(_pauseShortcut, SIGNAL(activated()),
            _glWidget, SLOT(slotPause()));
    // add layer
    _addLayerShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    _addLayerShortcut->setContext(Qt::ApplicationShortcut);
    connect(_addLayerShortcut, SIGNAL(activated()),
            _addLayerAction, SLOT(trigger()));
    // save map
    _saveMapShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    _saveMapShortcut->setContext(Qt::ApplicationShortcut);
    connect(_saveMapShortcut, SIGNAL(activated()),
            _saveMapFileAction, SLOT(trigger()));
    // open map
    _openMapShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    _openMapShortcut->setContext(Qt::ApplicationShortcut);
    connect(_openMapShortcut, SIGNAL(activated()),
            _openMapFileAction, SLOT(trigger()));
    // new map
    _newMapShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    _newMapShortcut->setContext(Qt::ApplicationShortcut);
    connect(_newMapShortcut, SIGNAL(activated()),
            _newMapAction, SLOT(trigger()));
}

void
MainWindow::loadTrackFile(const QString &trackFilePath)
{
    QList<Track*> *tracks = new QList<Track*>();
    _trackFileReader->readDeferred(trackFilePath, tracks);
}

bool
MainWindow::loadMapFile(const QString &path)
{
    _fileIO->setFilename(path);
    _fileIO->loadMapFile();
    QString trackFile;
    foreach(trackFile, _fileIO->getTrackFiles()) {
        loadTrackFile(trackFile);
    }
    return true;
}

void
MainWindow::clearMap()
{
    _fileIO->clear();
    _glWidget->getMap()->clearLayers();
    _layerListWidget->clear();
    _loadBaseMap();
    _playbackWidget->setTimeSliderMaximum(1800); /* 30 minutes */
    _glWidget->update();
}

bool
MainWindow::saveMapFile(const QString &path)
{
    _fileIO->setFilename(path);
    return _fileIO->writeMapFile();
}

bool
MainWindow::confirmAbandonMap()
{
    if (_fileIO->isDirty()) {
        QMessageBox::StandardButton res = QMessageBox::question(this,
            "Save changes to map?",
            "Map has unsaved changes, save it or discard?",
            QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,
            QMessageBox::Save);
        if (res == QMessageBox::Cancel)
            return false;
        else if (res == QMessageBox::Save)
            return slotSaveMapFile();
    }
    return true;
}

QString
MainWindow::getNetworkCacheDir() const
{
    QString cacheLoc = QStandardPaths::writableLocation
                                                (QStandardPaths::CacheLocation);
    if (cacheLoc.isEmpty()) {
        cacheLoc = QDir::homePath()+"/."+QCoreApplication::applicationName();
    }
    return cacheLoc;
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void
MainWindow::saveSettings()
{
    _settings->saveWidgetState(_glWidget);
    _settings->saveWidgetState(_playbackWidget);
    _settings->saveWidgetState(_layerListWidget);
}

void
MainWindow::restoreSettings()
{
    _settings->restoreWidgetState(_glWidget);
    _settings->restoreWidgetState(_playbackWidget);
    _settings->restoreWidgetState(_layerListWidget);
}

bool
MainWindow::slotSaveMapFile()
{
    if (_fileIO->getFilename().isEmpty())
        return slotSaveMapFileAs();
    _fileIO->writeMapFile();
    return true;
}

bool
MainWindow::slotSaveMapFileAs()
{
    QString saveFileName = QFileDialog::getSaveFileName(this);
    if (saveFileName.isEmpty())
        return false;
    return saveMapFile(saveFileName);
}

bool
MainWindow::slotOpenMapFile()
{
    if (!confirmAbandonMap())
        return false;
    QString path = QFileDialog::getOpenFileName(this, "Select map file");
    /* pump the event loop once to let the dialog disappear */
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (path.isEmpty())
        return false;
    clearMap();
    return loadMapFile(path);
}

void
MainWindow::slotNewMap()
{
    if (!confirmAbandonMap())
        return;
    clearMap();
}

void
MainWindow::_loadBaseMap()
{
    _glWidget->getMap()->addLayer(new OsmLayer());
}

void
MainWindow::slotAddLayer()
{
    QStringList paths = QFileDialog::getOpenFileNames(this,
                                        "Select track files (.gpx, .tcx, .fit)",
                                        QString() /*dir*/,
                                        "Tracklogs (*.gpx *.tcx *.fit)");
    if (!paths.isEmpty()) {
        QString path;
        foreach(path, paths) {
            loadTrackFile(path);
            _fileIO->addTrackFile(path);
            /* remember the last layer path explicitly added because we'll 
               frame up all tracks in it when the map is ready */
            _lastLayerPathAdded = path;
        }
    }
}

void
MainWindow::slotPlaybackRateChanged(const QString &newRate)
{
    QString rate(newRate);
    if (rate.endsWith("x"))
        rate.chop(1);
    bool ok = false;
    double theRate = rate.toDouble(&ok);
    if (ok)
        _glWidget->setPlaybackRate(theRate);
}

void
MainWindow::slotTimeChanged(double mapSeconds)
{
    _playbackWidget->setTime(mapSeconds);
}

void
MainWindow::onTimeSliderDrag(int seconds)
{
    _glWidget->setMapSeconds((double)seconds);
}

void
MainWindow::slotLayerAdded(LayerId layerId)
{
    int dur = _glWidget->getMap()->getDuration();
    if (dur > 0)
        _playbackWidget->setTimeSliderMaximum(dur);
    Layer *layer = _glWidget->getMap()->getLayer(layerId);
    if (layer) {
        _layerListWidget->addLayer(layer);
        
        if (!_lastLayerPathAdded.isEmpty() &&
            layer->sourceFilePath() == _lastLayerPathAdded)
            _layersToFrame.append(layer->id());
    }
    
    /* If the user explicitly added a path and the map is done projecting,
       frame all the layers that came from that path.
     */
    if (!_layersToFrame.empty() && _layersToFrame.length() == _numPendingLayers)
    {
        slotFrameLayers(_layersToFrame);
        _layerListWidget->slotSetSelectedLayers(_layersToFrame);
        _lastLayerPathAdded.clear();
        _layersToFrame.clear();
        _numPendingLayers = 0;
    }
    /* pump the event loop */
    qApp->processEvents();
}

void
MainWindow::slotFrameLayers(const QList<LayerId> layerIds)
{
    _glWidget->slotFrameLayers(layerIds);
}

void
MainWindow::slotLayerSelectionChanged(const QList<unsigned int> layerIds)
{
    _glWidget->slotSelectLayers(layerIds);
}

void
MainWindow::slotLayerVisibilityChanged(LayerId layerId, bool visible)
{
    Layer *layer = _glWidget->getMap()->getLayer(layerId);
    if (layer)
        layer->setVisible(visible);
    _glWidget->update();
}

void
MainWindow::slotShowAboutDialog()
{
    _aboutDialog->show();
}

void
MainWindow::slotShowPlaybackWidget()
{
    _showWidget(_playbackWidget);
}

void
MainWindow::slotShowLayerListWidget()
{
    _showWidget(_layerListWidget);
}

void
MainWindow::slotShowMapWindow()
{
    _showWidget(_glWidget);
}

void
MainWindow::slotTrackFileLoaded(const QString &path, QList<Track*> *tracks)
{
    _trackFiles.append(path);
    Track *thisTrack;
    QList<Layer*> trackLayers;
    foreach(thisTrack, *tracks) {
        TrackLayer *thisLayer = new TrackLayer(thisTrack);
        // See if the layer came from an explicitly-added path
        if (!_lastLayerPathAdded.isEmpty() &&
            thisLayer->sourceFilePath() == _lastLayerPathAdded)
            _numPendingLayers++;
        trackLayers.append(thisLayer);
        qApp->processEvents();
    }
    _glWidget->getMap()->addLayers(trackLayers);
}

void
MainWindow::slotTrackFileLoadError(const QString &path, const QString &what)
{
    QString err = QString("Error loading file '%0': %1").arg(path).arg(what);
    QMessageBox::critical(_glWidget, "Could not load file", err);
}

void
MainWindow::slotLockViewToSelectedLayer()
{
    QList<LayerId> layers = _layerListWidget->selectedLayerIds();
    if (layers.empty())
        QMessageBox::critical(_glWidget, "No layers selected",
                              "No layers selected to follow");
    else
        _glWidget->slotLockViewToLayer(layers.last());
}

void
MainWindow::slotFrameSelectedLayers()
{
    QList<LayerId> layers = _layerListWidget->selectedLayerIds();
    if (layers.empty())
        QMessageBox::critical(_glWidget, "No layers selected",
                              "No layers selected to frame");
    else
        slotFrameLayers(layers);
}

void
MainWindow::_showWidget(QWidget *widget)
{
    widget->show();
    widget->raise();
    widget->setWindowState(Qt::WindowActive);
}
