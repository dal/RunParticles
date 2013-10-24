//
//  GLWidget.h
//  RunParticles
//
//  Created by Doug Letterman on 9/16/13.
//
//

#ifndef __RunParticles__GLWidget__
#define __RunParticles__GLWidget__

#include <QElapsedTimer>
#include <QGLWidget>
#include <QMouseEvent>
#include <QSize>
#include <QTimer>
#include <iostream>

#include "cinder/Camera.h"

#include "Map.h"
#include "MapView.h"
#include "TimeCtx.h"

enum PlayMode {
    PlayMode_Pause,
    PlayMode_Play
};

class GLWidget : public QGLWidget
{
    
    Q_OBJECT
    
public:
    GLWidget(Map *map, QWidget *parent=0);
    
    virtual ~GLWidget();
    
    void initializeGL();
    
    QSize minimumSizeHint() const;
    
    QSize sizeHint() const;
    
    void paintGL();
    
    void resizeGL(int width, int height);
    
    void mousePressEvent(QMouseEvent *event);
    
    void mouseMoveEvent(QMouseEvent *event);
    
    void wheelEvent(QWheelEvent *event);
    
    MapPoint screenPointToRelativeMapPoint(const QPoint &pt);
    
    MapPoint screenPointToMapPoint(const QPoint &pt);
    
    Map* getMap() const;
    
public slots:
    
    void update();

    void slotPlay();
    
    void slotReverse();
    
    void slotPause();
    
    void slotRewind();
    
    void setPlaybackRate(double rate);
    
signals:
    
    void signalTimeChanged(double);
    
protected:
    
    void _updateViewCtx();
    
    PlayMode _playMode;
    
    qint64 _currentSeconds;
    
    CameraOrtho _camera;
    
    QPoint _lastPos;
    
    bool _inDrag;
    
    MapView _mapView;
    
    Map *_map;
    
    TimeCtx *_timeCtx;
    
    QElapsedTimer elapsedTimer;
    
    QTimer *timer;

};

#endif /* defined(__RunParticles__GLWidget__) */
