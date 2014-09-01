//
//  GLWidget.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/16/13.
//
//

#include "GLWidget.h"

#include <QWindow>

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

#include <set>

GLWidget::GLWidget(Map *map, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    _playMode(PlayMode_Pause),
    _map(map),
    _timer(new QTimer(this)),
    _idleTimer(new QTimer(this)),
    _fullScreen(false),
    _lockToLayer(false)
{
    elapsedTimer.start();
    // For now center the camera on Oakland
    MapPoint r = _map->getProjection().toProjection(LonLat(-122.2, 37.81155));
    MapPoint l = _map->getProjection().toProjection(LonLat(-122.3, 37.81155));
    float viewheight = (r.x - l.x) * ((float)height() / (float)width());
    _camera = CameraOrtho(l.x,
                          r.x,
                          r.y - viewheight*0.5,
                          r.y + viewheight*0.5,
                          -1,
                          1);
    _viewCamera = _camera;
    _mapView.setCurrentCam(_camera);
    _updateViewCtx();
    connect(_map, SIGNAL(signalLayerAdded(LayerId)),
            this, SLOT(updateGL()));
    connect(_map, SIGNAL(signalLayerClicked(LayerId)),
            this, SLOT(slotLayerSelected(LayerId)));
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(_map, SIGNAL(layerUpdated()), SLOT(slotRedrawWhenReady()));
    _timer->setInterval(Refresh_Interval);
    _idleTimer->setSingleShot(true);
    connect(_idleTimer, SIGNAL(timeout()), SLOT(updateGL()));
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
	gl::setMatrices( _viewCamera );
    
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    _map->draw(_viewCtx, _timeCtx);
    
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
    int oldWidth = _viewCtx.getViewportWidth();
    int oldHeight = _viewCtx.getViewportHeight();
    _mapView.resize(oldWidth, oldHeight, width, height);
    _updateViewCtx();
    glViewport (0, 0, (GLint) width, (GLint) height);
}

void
GLWidget::mousePressEvent(QMouseEvent *event)
{
    _in_mouseClick = true;
    _lastDownPos = event->pos();
    event->accept();
}

void
GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (_in_mouseClick && event->pos() == _lastDownPos) {
        _in_mouseClick = false;
        _onMouseClicked(QPoint(_lastDownPos.x(), height() - _lastDownPos.y()));
    }
    event->accept();
}

void
GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() != Qt::NoButton) {
        slotUnlockView();
        QPoint delta = event->pos() - _lastDownPos;
        MapPoint mapDelta = screenPointToRelativeMapPoint(delta);
        _mapView.mouseDrag(Vec2f(-mapDelta.x, mapDelta.y),
                           bool(event->buttons() & Qt::LeftButton),
                           bool(event->buttons() & Qt::MiddleButton),
                           bool(event->buttons() & Qt::RightButton));
        _updateViewCtx();
        updateGL();
        _lastDownPos = event->pos();
    }
    event->accept();
}

void
GLWidget::wheelEvent(QWheelEvent *event)
{
    _mapView.mouseWheel(-event->delta());
    _updateViewCtx();
    updateGL();
    qDebug("res: %f", _viewCtx.getResolution());
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
        _timeCtx.update(elapsedTimer.restart());
    
    // If the view is locked to a layer, recenter
    if (_lockToLayer) {
        const Layer *myLayer = _map->getLayer(_lockedLayer);
        if (myLayer) {
            _mapView.recenter(myLayer->position());
            _updateViewCtx();
        }
    }
        
    updateGL();
    emit signalTimeChanged(_timeCtx.getMapSeconds());
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
    _updateViewCtx();
    updateGL();
}

void
GLWidget::slotPlay()
{
    if (_timeCtx.getPlaybackRate() <= 0)
        _timeCtx.setPlaybackRate(-_timeCtx.getPlaybackRate());
    _playMode = PlayMode_Play;
    elapsedTimer.restart();
    _timer->start();
}

void
GLWidget::slotPause()
{
    _playMode = PlayMode_Pause;
    elapsedTimer.invalidate();
    _timer->stop();
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
    if (_timeCtx.getPlaybackRate() >= 0)
        _timeCtx.setPlaybackRate(-_timeCtx.getPlaybackRate());
    elapsedTimer.restart();
    _timer->start();
}

void
GLWidget::slotRewind()
{
    _timeCtx.setMapSeconds(0.);
    elapsedTimer.restart();
    update();
}

void
GLWidget::setMapSeconds(double seconds)
{
    _timeCtx.setMapSeconds(seconds);
    update();
}

void
GLWidget::setPlaybackRate(double rate)
{
    _timeCtx.setPlaybackRate(rate);
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
        if (layer)
            bbox += layer->getBoundingBox();
    }
    if (bbox.valid())
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
    if (newSel != _viewCtx.selectedLayers) {
        _viewCtx.selectedLayers = newSel;
        update();
    }
}

void
GLWidget::slotLockViewToLayer(LayerId layerId)
{
    _lockToLayer = true;
    _lockedLayer = layerId;
    update();
}

void
GLWidget::slotUnlockView()
{
    _lockToLayer = false;
}

void
GLWidget::_updateViewCtx()
{
    float left, top, right, bottom;
    qreal pixelRatio = windowHandle()->devicePixelRatio();
    _mapView.getFrustum(left, top, right, bottom);
    _viewCtx.setViewport(MapPoint(left, top),
                         MapPoint(right, bottom),
                         width() * pixelRatio,
                         height() * pixelRatio);
    _viewCamera.setOrtho(0., right-left, 0., top-bottom, -1, 1);
    _viewCtx.setCameraToWorld(Vec2d(left, bottom));
}

void
GLWidget::_onMouseClicked(QPoint pos)
{
    MapPoint clickPoint = screenPointToMapPoint(pos);
    if (!_map->onMapClicked(clickPoint, _viewCtx)) {
        QList<LayerId> layers;
        emit(signalLayersSelected(layers));
    }
}

void
GLWidget::slotLayerSelected(LayerId layerId)
{
    QList<LayerId> layers;
    layers << layerId;
    emit(signalLayersSelected(layers));
}

void
GLWidget::slotRedrawWhenReady()
{
    // update the GL view when idle
    if (!_timer->isActive() && !_idleTimer->isActive())
        _idleTimer->start(0);
}

