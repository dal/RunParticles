//
//  DemoPlayer.h
//  RunParticles
//
//  Created by Doug Letterman on 10/7/14.
//
//

#ifndef __RunParticles__DemoPlayer__
#define __RunParticles__DemoPlayer__

#include <QMediaPlayer>
#include <QObject>
#include <QTimer>

class MainWindow;

class DemoPlayer : public QObject
{
    
    Q_OBJECT
    
public:
    DemoPlayer(MainWindow *mainWindow);
    
protected:
    MainWindow *_mainWindow;
    
    QTimer *_timer;
    
    unsigned int _position;
    
    QMediaPlayer *_audioPlayer;
    
protected slots:
    void play();
    void playMusic();
    
};
#endif /* defined(__RunParticles__DemoPlayer__) */
