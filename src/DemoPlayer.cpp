//
//  DemoPlayer.cpp
//  RunParticles
//
//  Created by Doug Letterman on 10/7/14.
//
//

#include <QFile>

#include "DemoPlayer.h"
#include "MainWindow.h"


DemoPlayer::DemoPlayer(MainWindow *mainWindow) :
    QObject(mainWindow),
    _mainWindow(mainWindow),
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
}

void
DemoPlayer::play()
{
    int _interval = 1000;
    switch (_position)
    {
        case 0:
            _mainWindow->slotMaximizeGLWidget();
            _mainWindow->slotPlaybackRateChanged("64x");
            _mainWindow->loadMapFile("/Users/dal/tmp/helloWorld");
            _interval = -1;
            break;
        case 1:
            playMusic();
            _mainWindow->slotPlay();
            break;
            
    };
    if (_interval > 0)
        _timer->setInterval(_interval);
    else
        _timer->stop();
    _position++;
}

void DemoPlayer::playMusic()
{
    QFile::copy(":/music" , "/tmp/tontura.mp3");
    _audioPlayer->setMedia(QUrl::fromLocalFile("/tmp/tontura.mp3"));
    _audioPlayer->setVolume(50);
    _audioPlayer->play();
}


