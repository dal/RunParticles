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

#include "OsmTileSource.h"

class MainWindow;

class GLWidget;

class DemoPlayer : public QObject
{
    
    Q_OBJECT
    
public:
    DemoPlayer(MainWindow *mainWindow, GLWidget *glWidget);
    
    virtual ~DemoPlayer();
    
protected:
    void _copyResources();
    
    void _cleanupResources();
    
    MainWindow *_mainWindow;
    
    GLWidget *_glWidget;
    
    QTimer *_timer;
    
    unsigned int _position;
    
    QMediaPlayer *_audioPlayer;
    
    static QList<OsmIndex> cacheList1, cacheList2, cacheList3;
    
protected slots:
    void play();
    void playMusic();
    
};
#endif /* defined(__RunParticles__DemoPlayer__) */
