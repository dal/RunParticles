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

class GLWidget;

class DemoPlayer : public QObject
{
    
    Q_OBJECT
    
public:
    DemoPlayer(MainWindow *mainWindow, GLWidget *glWidget);
    
protected:
    void _copyResources();
    
    MainWindow *_mainWindow;
    
    GLWidget *_glWidget;
    
    QTimer *_timer;
    
    unsigned int _position;
    
    QMediaPlayer *_audioPlayer;
    
protected slots:
    void play();
    void playMusic();
    
};
#endif /* defined(__RunParticles__DemoPlayer__) */
