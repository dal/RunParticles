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
#include <iostream>

#include "cinder/Camera.h"

#include "Map.h"
#include "MapView.h"
#include "TimeCtx.h"

class GLWidget : public QGLWidget
{
    
public:
    GLWidget(QWidget *parent=0);
    
    virtual ~GLWidget();
    
    void initializeGL();
    
    QSize minimumSizeHint() const;
    
    QSize sizeHint() const;
    
    void paintGL();
    
    void resizeGL(int width, int height);
    
    void mousePressEvent(QMouseEvent *event);
    
    void mouseMoveEvent(QMouseEvent *event);
    
    void wheelEvent(QWheelEvent *event);
    
    void update();
    
    MapPoint screenPointToMapPoint(const QPoint &pt);
    
protected:
    
    qint64 _currentSeconds;
    
    CameraOrtho _camera;
    
    MapPoint lastPos;
    
    MapView _mapView;
    
    Map *_map;
    
    TimeCtx _timeCtx;
    
    QElapsedTimer timer;

};

#endif /* defined(__RunParticles__GLWidget__) */
