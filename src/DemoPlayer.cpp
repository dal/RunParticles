//
//  DemoPlayer.cpp
//  RunParticles
//
//  Created by Doug Letterman on 10/7/14.
//
//

#include <QApplication>
#include <QFile>

#include "DemoPlayer.h"
#include "GLWidget.h"
#include "MainWindow.h"


DemoPlayer::DemoPlayer(MainWindow *mainWindow, GLWidget *glWidget) :
    QObject(mainWindow),
    _mainWindow(mainWindow),
    _glWidget(glWidget),
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
    _copyResources();
}
               
void
DemoPlayer::_copyResources()
{
    QFile::copy(":/music" , "/tmp/tontura.mp3");
    QFile::copy(":/../resources/map.xml" , "/tmp/map.xml");
}

void
DemoPlayer::play()
{
    int _interval = 1000;
    switch (_position)
    {
        case 0:
            _glWidget->showMaximized();
            _glWidget->setFocus();
            _mainWindow->slotPlaybackRateChanged("64x");
            _mainWindow->loadMapFile("/Users/dal/tmp/helloWorld");
            _interval = -1;
            break;
        case 1:
            playMusic();
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
            //qApp->quit();
            _interval = -1;
            break;
            
    };
    if (_interval > 0)
        _timer->start(_interval);
    else
        _timer->stop();
    _position++;
}

void DemoPlayer::playMusic()
{
    _audioPlayer->setMedia(QUrl::fromLocalFile("/tmp/tontura.mp3"));
    _audioPlayer->setVolume(50);
    _audioPlayer->play();
}


