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

#include "BoundingBox.h"
#include "Map.h"
#include "MapView.h"
#include "TimeCtx.h"

class GLWidget : public QGLWidget
{

    Q_OBJECT
    
public:
    
    enum PlayMode {
        PlayMode_Pause,
        PlayMode_Play
    };
    
    enum {
        Refresh_Interval = 15
    };

    
    GLWidget(Map *map, QWidget *parent=0);
    
    virtual ~GLWidget();
    
    void initializeGL();
    
    QSize minimumSizeHint() const;
    
    QSize sizeHint() const;
    
    void paintGL();
    
    void resizeGL(int width, int height);
    
    void mousePressEvent(QMouseEvent *event);
    
    void mouseReleaseEvent(QMouseEvent *event);
    
    void mouseMoveEvent(QMouseEvent *event);
    
    void wheelEvent(QWheelEvent *event);
    
    void keyPressEvent(QKeyEvent *event);
    
    MapPoint screenPointToRelativeMapPoint(const QPoint &pt);
    
    MapPoint screenPointToMapPoint(const QPoint &pt);
    
    Map* getMap() const;
    
    void frameBoundingBox(const BoundingBox &bbox);
    
    void mouseWheelZoom(float delta);
    
    void mouseDragZoom(const QPoint &screenDelta);
    
    /*
     Scales the viewport by by the specified multiplier amount. Zoom amount 0.9
     would scale the mapview down by 10%, effectively zooming in. A zoom amount
     greater than 1.0 represents a zoom out.
     */
    void zoom(float amount);
    
    void moveView(const QPoint &screenDelta);
    
public slots:
    
    void update();

    void slotPlay();
    
    void slotTogglePlayPause();
    
    void slotPause();
    
    void slotReverse();
    
    void slotRewind();
    
    void setMapSeconds(double seconds);
    
    void setPlaybackRate(double rate);
    
    void slotFrameLayers(QList<LayerId> layerIds);
    
    void slotSelectLayers(QList<LayerId> layerIds);
    
    void slotLockViewToLayer(LayerId layerId);
    
    void slotUnlockView();
    
    void slotLayerSelected(LayerId layerId);
    
    void slotRedrawWhenReady();
    
signals:
    
    void signalTimeChanged(double);
    
    void signalLayerClicked(LayerId);
    
    void signalLayersSelected(QList<LayerId>);
    
protected:
    
    void _updateViewCtx();
    
    void _onMouseClicked(QPoint pos);
    
    PlayMode _playMode;
    
    qint64 _currentSeconds;
    
    CameraOrtho _viewCamera;
    
    QPoint _lastDownPos;
    
    MapView _mapView;
    
    Map *_map;
    
    TimeCtx _timeCtx;
    
    ViewCtx _viewCtx;
    
    QElapsedTimer elapsedTimer;
    
    QTimer *_timer, *_idleTimer;
    
    bool _fullScreen;
    
    bool _lockToLayer;
    
    bool _in_mouseClick;
    
    LayerId _lockedLayer;

};

#endif /* defined(__RunParticles__GLWidget__) */
