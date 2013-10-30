#include "MainWindow.h"
#include "TcxHandler.h"
#include "TrackLAyer.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

MainWindow::MainWindow(GLWidget *glWidget,
                       QWidget * parent,
                       Qt::WindowFlags flags) :
    QMainWindow::QMainWindow(parent, flags),
    _menuBar(new QMenuBar(0)),
    _glWidget(glWidget)
{
    QWidget *ctr = new QWidget(this);
    setCentralWidget(ctr);
    _rewindButton = new QPushButton("<<", ctr);
    _backButton = new QPushButton("<", ctr);
    _pauseButton = new QPushButton("x",ctr);
    _forwardButton = new QPushButton(">", ctr);
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
    _openLayerAction = new QAction("Open Layer. . .", this);
    _forwardAction = new QAction("Play", this);
    _backAction = new QAction("Reverse", this);
    _rewindAction = new QAction("Rewind", this);
    _pauseAction = new QAction("Pause", this);
    _fileMenu->addAction(_openLayerAction);
    connect(_openLayerAction, SIGNAL(triggered(bool)),
            this, SLOT(slotLoadFile()));
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
    
    connect(_glWidget, SIGNAL(signalTimeChanged(double)),
            this, SLOT(slotTimeChanged(double)));
    connect(_glWidget->getMap(), SIGNAL(signalLayerAdded()),
            this, SLOT(slotLayerAdded()));
    
    slotTimeChanged(0);
    // DEBUG
    loadTcxFile(new QFile("/Users/dal/Dropbox/tmp/10-8-13 6-29-18 PM.tcx"));
    loadTcxFile(new QFile("/Users/dal/Documents/gps/exports/all2012.tcx"));
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
MainWindow::loadTcxFile(QFile *tcxFile)
{
    QList<Track*> tracks;
    TcxHandler *handler = new TcxHandler(&tracks);
    QXmlSimpleReader *reader = new QXmlSimpleReader();
    reader->setContentHandler(handler);
    reader->setErrorHandler(handler);
    
    QString msg = QString("File '%1' does not exist").arg(tcxFile->fileName());
    
    if (!tcxFile->exists()) {
        QMessageBox::critical(this, msg, msg);
    }
    
    QXmlInputSource *source = new QXmlInputSource(tcxFile);
    
    reader->parse(source, true /*incremental*/);
    while (reader->parseContinue()) { };
    Track *thisTrack;
    foreach(thisTrack, tracks) {
        TrackLayer *thisLayer = new TrackLayer(thisTrack);
        _glWidget->getMap()->addLayer(thisLayer);
    }
}

void
MainWindow::slotLoadFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Select tcx file");
    if (!path.isEmpty()) {
        QFile *tcxFile = new QFile(path);
        loadTcxFile(tcxFile);
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
    int hours = mapSeconds / 3600;
    int modhrs = int(mapSeconds) % 3600;
    int mins = modhrs / 60;
    int secs = modhrs % 60;
    QChar zero('0');
    QString time = QString("%1hr%2m%3s").arg(hours)
                                        .arg(mins, 2, 10, zero)
                                        .arg(secs, 2, 10, zero);
    _currentTimeLineEdit->setText(time);
    _slider->setSliderPosition(int(mapSeconds));
}

void
MainWindow::slotLayerAdded()
{
    int dur = _glWidget->getMap()->getDuration();
    _slider->setMaximum(dur);
}
