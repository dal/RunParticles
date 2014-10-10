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

<<<<<<< HEAD
class GLWidget;

=======
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
class DemoPlayer : public QObject
{
    
    Q_OBJECT
    
public:
<<<<<<< HEAD
    DemoPlayer(MainWindow *mainWindow, GLWidget *glWidget);
    
    virtual ~DemoPlayer();
    
protected:
    void _copyResources();
    
    void _cleanupResources();
    
    MainWindow *_mainWindow;
    
    GLWidget *_glWidget;
    
=======
    DemoPlayer(MainWindow *mainWindow);
    
protected:
    MainWindow *_mainWindow;
    
>>>>>>> Added DemoPlayer class and methods to play back some silly music.
    QTimer *_timer;
    
    unsigned int _position;
    
    QMediaPlayer *_audioPlayer;
    
protected slots:
    void play();
    void playMusic();
    
};
#endif /* defined(__RunParticles__DemoPlayer__) */
