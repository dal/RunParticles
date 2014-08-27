#include "MainWindow.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QEventLoop>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>

#include "MapFileIO.h"
#include "OsmLayer.h"
#include "TcxHandler.h"
#include "TrackLayer.h"
#include "TrackFileReader.h"
#include "Util.h"

MainWindow::MainWindow(GLWidget *glWidget,
                       QWidget * parent,
                       Qt::WindowFlags flags) :
    QMainWindow::QMainWindow(parent, flags),
    _fileIO(new MapFileIO(this)),
    _menuBar(new QMenuBar(0)),
    _glWidget(glWidget),
    _layerListWidget(new LayerListWidget())
{
    _networkAccessManager = Singleton<QNetworkAccessManager>::Instance();
    _networkAccessManager->setParent(this);
    _diskCache = new QNetworkDiskCache(this);
    QString cacheDir = getNetworkCacheDir();
    _diskCache->setCacheDirectory(cacheDir);
    _networkAccessManager->setCache(_diskCache);
    
    QWidget *ctr = new QWidget(this);
    setCentralWidget(ctr);
    _rewindButton = new QPushButton(QString::fromUtf8("\u25C0\u25C0"), ctr);//<<
    _backButton = new QPushButton(QString::fromUtf8("\u25C0"), ctr); // <
    _pauseButton = new QPushButton(QString::fromUtf8("\u2588"),ctr); // [ ]
    _forwardButton = new QPushButton(QString::fromUtf8("\u25BA"), ctr); // >
    _playSpeedCombo = new QComboBox(ctr);
    _playSpeedCombo->addItem("1x");
    _playSpeedCombo->addItem("2x");
    _playSpeedCombo->addItem("4x");
    _playSpeedCombo->addItem("8x");
    _playSpeedCombo->addItem("16x");
    _playSpeedCombo->addItem("32x");
    _playSpeedCombo->addItem("64x");
    _slider = new QSlider(Qt::Horizontal, ctr);
    _currentTimeLineEdit = new QLineEdit(ctr);
    _currentTimeLineEdit->setReadOnly(true);
    _currentTimeLineEdit->setText("0");
    _currentTimeLineEdit->setAlignment(Qt::AlignRight);
    _layout(ctr);
    QMenu *_fileMenu = _menuBar->addMenu("File");
    _newMapAction = new QAction("&New Map", this);
    _openMapFileAction = new QAction("&Open Map. . .", this);
    _saveAsMapFileAction = new QAction("Save Map As. . .", this);
    _saveMapFileAction = new QAction("&Save Map", this);
    _addLayerAction = new QAction("&Add Track File. . .", this);
    _forwardAction = new QAction("Play", this);
    _backAction = new QAction("Reverse", this);
    _rewindAction = new QAction("Rewind", this);
    _pauseAction = new QAction("Pause", this);
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
    connect(_forwardButton, SIGNAL(clicked(bool)),
            _forwardAction, SLOT(trigger()));
    connect(_backButton, SIGNAL(clicked(bool)),
            _backAction, SLOT(trigger()));
    connect(_pauseButton, SIGNAL(clicked(bool)),
            _pauseAction, SLOT(trigger()));
    connect(_rewindButton, SIGNAL(clicked(bool)),
            _rewindAction, SLOT(trigger()));
    connect(_playSpeedCombo, SIGNAL(activated(const QString&)),
            this, SLOT(slotPlaybackRateChanged(const QString&)));
    
    connect(_forwardAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotPlay()));
    connect(_backAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotReverse()));
    connect(_pauseAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotPause()));
    connect(_rewindAction, SIGNAL(triggered()),
            _glWidget, SLOT(slotRewind()));
    
    connect(_slider, SIGNAL(valueChanged(int)),
            SLOT(onTimeSliderDrag(int)));
    
    connect(_glWidget, SIGNAL(signalTimeChanged(double)),
            this, SLOT(slotTimeChanged(double)));
    connect(_glWidget->getMap(), SIGNAL(signalLayerAdded()),
            this, SLOT(slotLayerAdded()));
    connect(_glWidget, SIGNAL(signalLayersSelected(QList<LayerId>)),
            _layerListWidget, SLOT(slotSetSelectedLayers(QList<LayerId>)));
    
    // Application-wide shortcuts
    _playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    _playPauseShortcut->setContext(Qt::ApplicationShortcut);
    connect(_playPauseShortcut, SIGNAL(activated()),
            _glWidget, SLOT(slotTogglePlayPause()));
    _addLayerShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    _addLayerShortcut->setContext(Qt::ApplicationShortcut);
    connect(_addLayerShortcut, SIGNAL(activated()),
            _addLayerAction, SLOT(trigger()));
    _saveMapShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    _saveMapShortcut->setContext(Qt::ApplicationShortcut);
    connect(_saveMapShortcut, SIGNAL(activated()),
            _saveMapFileAction, SLOT(trigger()));
    _openMapShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    _openMapShortcut->setContext(Qt::ApplicationShortcut);
    connect(_openMapShortcut, SIGNAL(activated()),
            _openMapFileAction, SLOT(trigger()));
    _newMapShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    _newMapShortcut->setContext(Qt::ApplicationShortcut);
    connect(_newMapShortcut, SIGNAL(activated()),
            _newMapAction, SLOT(trigger()));
    
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
    _layerListWidget->show();
    
    _loadBaseMap();
}

MainWindow::~MainWindow()
{
    // empty
}

void
MainWindow::_layout(QWidget *ctr)
{
    QGridLayout *_layout = new QGridLayout(ctr);
    _layout->addWidget(_rewindButton, 0, 0);
    _layout->addWidget(_backButton, 0, 1);
    _layout->addWidget(_pauseButton, 0, 2);
    _layout->addWidget(_forwardButton, 0, 3);
    _layout->addWidget(_playSpeedCombo, 0, 4);
    _layout->addWidget(_slider, 1, 0, 1, 4);
    _layout->addWidget(_currentTimeLineEdit, 1, 4);
    for (int i=0; i < _layout->columnCount(); i++)
        _layout->setColumnStretch(i, 1);
}

void
MainWindow::loadTrackFile(const QString &path)
{
    QList<Track*> tracks;
    char* whyNot = (char*)malloc(256);
    TrackFileReader reader;
    bool success = reader.read(path, &tracks, &whyNot);
    if (!success) {
        QMessageBox::critical(this, "Could not load file", QString(whyNot));
        return;
    }
    _trackFiles.append(path);
    Track *thisTrack;
    foreach(thisTrack, tracks) {
        TrackLayer *thisLayer = new TrackLayer(thisTrack);
        _glWidget->getMap()->addLayer(thisLayer);
        _layerListWidget->addLayer(thisLayer);
    }
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
    if (path.isEmpty())
        return false;
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
                                                      "Select track file",
                                                      QString() /*dir*/,
                                                      "Tracklogs (*.gpx *.tcx)");
    if (!paths.isEmpty()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QString path;
        foreach(path, paths) {
            loadTrackFile(path);
            _fileIO->addTrackFile(path);
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
    QString time = Util::secondsToString(mapSeconds);
    _currentTimeLineEdit->setText(time);
    _slider->blockSignals(true);
    _slider->setSliderPosition(int(mapSeconds));
    _slider->blockSignals(false);
}

void
MainWindow::onTimeSliderDrag(int seconds)
{
    _glWidget->setMapSeconds((double)seconds);
}

void
MainWindow::slotLayerAdded()
{
    int dur = _glWidget->getMap()->getDuration();
    _slider->setMaximum(dur);
}

void
MainWindow::slotFrameLayers(const QList<unsigned int> layerIds)
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
    layer->setVisible(visible);
}
