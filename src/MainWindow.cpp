#include "MainWindow.h"
#include "TcxHandler.h"

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
    connect(_openLayerAction, SIGNAL(triggered(bool)),
            this, SLOT(slotLoadFile()));
}

MainWindow::~MainWindow()
{
    // empty
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
