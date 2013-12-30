//
//  GLWidget.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/16/13.
//
//

#include "GLWidget.h"

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

#include <set>

GLWidget::GLWidget(Map *map, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    _playMode(PlayMode_Pause),
    timer(new QTimer(this)),
    _fullScreen(false)
{
    setMouseTracking(true);
    elapsedTimer.start();
    _map = map;
    // For now center the camera on Oakland
    MapPoint r = _map->getViewCtx()->toProjection(LonLat(-122.2, 37.81155));
    MapPoint l = _map->getViewCtx()->toProjection(LonLat(-122.3, 37.81155));
    float viewheight = (r.x - l.x) * ((float)height() / (float)width());
    _camera = CameraOrtho(l.x,
                          r.x,
                          r.y - viewheight*0.5,
                          r.y + viewheight*0.5,
                          -1,
                          1);
    _mapView.setCurrentCam(_camera);
    _mapView.setViewCtx(_map->getViewCtx());
    _timeCtx = _map->getTimeCtx();
    _updateViewCtx();
    connect(_map, SIGNAL(signalLayerAdded()), this, SLOT(updateGL()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}

void
GLWidget::initializeGL()
{
    glClearColor(0, 0, 0, 0);
}

void
GLWidget::paintGL()
{
    _camera = _mapView.getCamera();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    
    // setup camera
	gl::pushMatrices();
	gl::setMatrices( _camera );
    
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color( Color( 1, 0, 0 ) );
    
    _map->draw();
    
    glPopMatrix();
}

GLWidget::~GLWidget()
{
    
}

QSize
GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize
GLWidget::sizeHint() const
{
    return QSize(800, 600);
}

void
GLWidget::resizeGL(int width, int height)
{
    int oldWidth = _map->getViewCtx()->getViewportWidth();
    int oldHeight = _map->getViewCtx()->getViewportHeight();
    _mapView.resize(oldWidth, oldHeight, width, height);
    _updateViewCtx();
    glViewport (0, 0, (GLdouble) width, (GLdouble) height);
}

void
GLWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void
GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint eventXy = QPoint(event->x(), height() - event->y());
    if (event->buttons() != Qt::NoButton) {
        _inDrag = true;
        QPoint delta = eventXy - _lastPos;
        MapPoint mapDelta = screenPointToRelativeMapPoint(delta);
        _mapView.mouseDrag(Vec2f(-mapDelta.x, -mapDelta.y),
                           bool(event->buttons() & Qt::LeftButton),
                           bool(event->buttons() & Qt::MiddleButton),
                           bool(event->buttons() & Qt::RightButton));
        _updateViewCtx();
        updateGL();
    }
    _lastPos = eventXy;
    event->accept();
}

void
GLWidget::wheelEvent(QWheelEvent *event)
{
    _mapView.mouseWheel(-event->delta());
    _updateViewCtx();
    qDebug("resolution: %f", _map->getViewCtx()->getResolution());
    updateGL();
}

void
GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F
        && event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))
    {
        if (_fullScreen) {
            _fullScreen = false;
            this->showNormal();
        } else {
            _fullScreen = true;
            this->showFullScreen();
        }
    }
    QGLWidget::keyPressEvent(event);
}

void
GLWidget::update()
{
    if (_playMode == PlayMode_Play && elapsedTimer.isValid())
        _timeCtx->update(elapsedTimer.restart());
    updateGL();
    emit signalTimeChanged(_timeCtx->getMapSeconds());
}

MapPoint
GLWidget::screenPointToRelativeMapPoint(const QPoint &pt)
{
    float left, top, right, bottom, near, far;
    // *left, *top, *right, *bottom, *near, *far
    _camera.getFrustum(&left,
                       &top,
                       &right,
                       &bottom,
                       &near,
                       &far);
    float xPixelsToMeters = (right - left) / (float)width();
    float yPixelsToMeters = (top - bottom) / (float)height();
    return MapPoint(pt.x()*xPixelsToMeters, pt.y()*yPixelsToMeters);
}

MapPoint
GLWidget::screenPointToMapPoint(const QPoint &pt)
{
    float left, top, right, bottom, near, far;
    // *left, *top, *right, *bottom, *near, *far
    _camera.getFrustum(&left,
                       &top,
                       &right,
                       &bottom,
                       &near,
                       &far);
    float xPixelsToMeters = (right - left) / (float)width();
    float yPixelsToMeters = (top - bottom) / (float)height();
    return MapPoint(left + pt.x()*xPixelsToMeters,
                    bottom + pt.y()*yPixelsToMeters);
}

Map*
GLWidget::getMap() const
{
    return _map;
}

void
GLWidget::frameBoundingBox(const BoundingBox &bbox)
{
    MapPoint center = bbox.center();
    double bboxRatio = bbox.height() / bbox.width();
    double viewRatio = ((double)height() / (double)width());
    if (bboxRatio < viewRatio) {
        double viewheight = bbox.width() * viewRatio;
        _camera = CameraOrtho(bbox.upperLeft.x,
                              bbox.lowerRight.x,
                              center.y - viewheight * 0.5,
                              center.y + viewheight * 0.5,
                              -1,
                              1);
    } else {
        float viewWidth = bbox.height() / viewRatio;
        _camera = CameraOrtho(center.x - viewWidth * 0.5,
                              center.x + viewWidth * 0.5,
                              bbox.lowerRight.y,
                              bbox.upperLeft.y,
                              -1,
                              1);
    }
    _mapView.setCurrentCam(_camera);
    _mapView.setViewCtx(_map->getViewCtx());
    _updateViewCtx();
    updateGL();
}

void
GLWidget::slotPlay()
{
    if (_timeCtx->getPlaybackRate() <= 0)
        _timeCtx->setPlaybackRate(-_timeCtx->getPlaybackRate());
    _playMode = PlayMode_Play;
    elapsedTimer.restart();
    timer->start();
}

void
GLWidget::slotPause()
{
    _playMode = PlayMode_Pause;
    elapsedTimer.invalidate();
    timer->stop();
}

void
GLWidget::slotTogglePlayPause()
{
    if (_playMode == PlayMode_Pause)
        slotPlay();
    else
        slotPause();
}

void
GLWidget::slotReverse()
{
    _playMode = PlayMode_Play;
    if (_timeCtx->getPlaybackRate() >= 0)
        _timeCtx->setPlaybackRate(-_timeCtx->getPlaybackRate());
    elapsedTimer.restart();
    timer->start();
}

void
GLWidget::slotRewind()
{
    _timeCtx->setMapSeconds(0.);
    elapsedTimer.restart();
    update();
}

void
GLWidget::setPlaybackRate(double rate)
{
    _timeCtx->setPlaybackRate(rate);
}

void
GLWidget::slotFrameLayers(QList<LayerId> layerIds)
{
    if (layerIds.empty())
        return;
    BoundingBox bbox;
    LayerId layerId;
    foreach(layerId, layerIds)
    {
        Layer* layer = _map->getLayer(layerId);
        bbox += layer->getBoundingBox();
    }
    frameBoundingBox(bbox);
}

void
GLWidget::slotSelectLayers(QList<LayerId> layerIds)
{
    std::set<LayerId> newSel;
    LayerId layerId;
    foreach(layerId, layerIds) {
        newSel.insert(layerId);
    }
    if (newSel != _map->getViewCtx()->selectedLayers) {
        _map->getViewCtx()->selectedLayers = newSel;
        update();
    }
}

void
GLWidget::_updateViewCtx()
{
    float left, top, right, bottom;
    _mapView.getFrustum(left, top, right, bottom);
    _map->getViewCtx()->setViewport(MapPoint(left, top),
                                    MapPoint(right, bottom),
                                    width(),
                                    height());
}

