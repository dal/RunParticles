//
//  DemoPlayer.cpp
//  RunParticles
//
//  Created by Doug Letterman on 10/7/14.
//
//

<<<<<<< HEAD
#include <QApplication>
#include <QFile>

#include "DemoPlayer.h"
#include "GLWidget.h"
#include "MainWindow.h"
#include "Types.h"

DemoPlayer::DemoPlayer(MainWindow *mainWindow, GLWidget *glWidget) :
    QObject(mainWindow),
    _mainWindow(mainWindow),
    _glWidget(glWidget),
=======
#include <QFile>

#include "DemoPlayer.h"
#include "MainWindow.h"


DemoPlayer::DemoPlayer(MainWindow *mainWindow) :
    QObject(mainWindow),
    _mainWindow(mainWindow),
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
    _timer(new QTimer(this)),
    _position(0),
    _audioPlayer(new QMediaPlayer(this))
{
    connect(_timer, &QTimer::timeout, this, &DemoPlayer::play);
    connect(_mainWindow, &MainWindow::signalDoneReading,
            this, &DemoPlayer::play);
    _timer->setSingleShot(true);
    _timer->setInterval(1000);
    _timer->start();
<<<<<<< HEAD
    _copyResources();
}

DemoPlayer::~DemoPlayer()
{
    _cleanupResources();
}
               
void
DemoPlayer::_copyResources()
{
    QFile::copy(":/music" , "/tmp/tontura.mp3");
    QFile::copy(":/resources/map.xml" , "/tmp/map.xml");
    QFile::copy(":/resources/all2010.tcx" , "/tmp/all2010.tcx");
    QFile::copy(":/resources/all2011.tcx" , "/tmp/all2011.tcx");
    QFile::copy(":/resources/all2012.tcx" , "/tmp/all2012.tcx");
    QFile::copy(":/resources/all2013.tcx" , "/tmp/all2013.tcx");
}

void
DemoPlayer::_cleanupResources()
{
    QFile::remove("/tmp/tontura.mp3");
    QFile::remove("/tmp/map.xml");
    QFile::remove("/tmp/all2010.tcx");
    QFile::remove("/tmp/all2011.tcx");
    QFile::remove("/tmp/all2012.tcx");
    QFile::remove("/tmp/all2013.tcx");
=======
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
}

void
DemoPlayer::play()
{
    int _interval = 1000;
    switch (_position)
    {
        case 0:
<<<<<<< HEAD
            //_glWidget->showMaximized();
            _glWidget->showFullScreen();
            _mainWindow->slotPlaybackRateChanged("64x");
            _mainWindow->loadMapFile("/tmp/map.xml");
=======
            _mainWindow->slotMaximizeGLWidget();
            _mainWindow->slotPlaybackRateChanged("64x");
            _mainWindow->loadMapFile("/Users/dal/tmp/helloWorld");
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
            _interval = -1;
            break;
        case 1:
            playMusic();
<<<<<<< HEAD
            _glWidget->slotPlay();
            _interval = 4000;
            break;
        case 2:
            _glWidget->slotLockViewToLayer(4);
            _glWidget->zoom(0.2);
            _interval = 8000;
            break;
        case 3:
            _glWidget->slotLockViewToLayer(18);
            _glWidget->zoom(2.0);
            _interval = 8000;
            break;
        case 4:
            _glWidget->slotLockViewToLayer(36);
            _interval = 6000;
            break;
        case 5:
            _glWidget->slotLockViewToLayer(605);
            _interval = 8000;
            break;
        case 6:
            _glWidget->setMapSeconds(0.);
            _glWidget->recenter(LonLat(-122.2750, 37.8689));
            _interval = 2000;
            break;
        case 7:
            _glWidget->zoom(1.5);
            _interval = 2000;
            break;
        case 8:
            _glWidget->zoom(1.5);
            _interval = 2000;
            break;
        case 9:
            _glWidget->zoom(1.5);
            _interval = 2000;
            break;
        case 10:
            _glWidget->zoom(1.5);
            _interval = 2000;
            break;
        case 11:
            _glWidget->zoom(1.5);
            _interval = 2000;
            break;
        case 12:
            _glWidget->zoom(1.5);
            _interval = 2000;
            break;
        case 13:
            //qApp->quit();
            _interval = -1;
=======
            _mainWindow->slotPlay();
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
            break;
            
    };
    if (_interval > 0)
<<<<<<< HEAD
        _timer->start(_interval);
=======
        _timer->setInterval(_interval);
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
    else
        _timer->stop();
    _position++;
}

void DemoPlayer::playMusic()
{
<<<<<<< HEAD
=======
    QFile::copy(":/music" , "/tmp/tontura.mp3");
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
    _audioPlayer->setMedia(QUrl::fromLocalFile("/tmp/tontura.mp3"));
    _audioPlayer->setVolume(50);
    _audioPlayer->play();
}


