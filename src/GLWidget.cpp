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
    setObjectName("GLWidget");
    if (_map == NULL)
        _map = new Map();
    elapsedTimer.start();
    connect(_map, SIGNAL(signalLayerClicked(LayerId)),
            this, SLOT(slotLayerSelected(LayerId)));
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(_map, SIGNAL(layerUpdated()), SLOT(slotRedrawWhenReady()));
    _timer->setInterval(Refresh_Interval);
    _idleTimer->setSingleShot(true);
    if (QWindow *window = windowHandle())
        _viewCtx.setDevicePixelRatio(window->devicePixelRatio());
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
    _lastMovePos = event->pos();
    _lastDownPos = _lastMovePos;
    event->accept();
}

void
GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (_in_mouseClick && event->pos() == _lastDownPos) {
        _in_mouseClick = false;
        _onMouseClicked(QPoint(_lastMovePos.x(), height() - _lastMovePos.y()));
    }
    event->accept();
}

void
GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    MapPoint mapPoint = screenPointToMapPoint(QPoint(event->x(),
                                                     height() - event->y()));
    _mapView.recenter(mapPoint);
    zoom(0.5);
    event->accept();
}

void
GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() != Qt::NoButton) {
        slotUnlockView();
        QPoint delta = event->pos() - _lastMovePos;
        if (event->buttons() & Qt::LeftButton) {
            moveView(delta);
        } else if (event->buttons() & Qt::MiddleButton ||
                   event->buttons() & Qt::RightButton)
        {
            mouseDragZoom(delta);
        }
        _lastMovePos = event->pos();
    }
    event->accept();
}

void
GLWidget::wheelEvent(QWheelEvent *event)
{
    mouseWheelZoom(-event->delta());
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
    } else if (event->key() == Qt::Key_Minus ||
               event->key() == Qt::Key_Underscore)
    {
        zoom(1.2);
    } else if (event->key() == Qt::Key_Equal ||
               event->key() == Qt::Key_Plus)
    {
        zoom(0.8);
    } else if (event->key() == Qt::Key_Left) {
        QPoint amt(width()*0.1, 0);
        if (event->modifiers() & Qt::ShiftModifier)
            amt *= 2;
        moveView(amt);
    } else if (event->key() == Qt::Key_Right) {
        QPoint amt(-width()*0.1, 0);
        if (event->modifiers() & Qt::ShiftModifier)
            amt *= 2;
        moveView(amt);
    } else if (event->key() == Qt::Key_Up) {
        QPoint amt(0, height()*0.1);
        if (event->modifiers() & Qt::ShiftModifier)
            amt *= 2;
        moveView(amt);
    } else if (event->key() == Qt::Key_Down) {
        QPoint amt(0, -height()*0.1);
        if (event->modifiers() & Qt::ShiftModifier)
            amt *= 2;
        moveView(amt);
    }
    QGLWidget::keyPressEvent(event);
}

void
GLWidget::update()
{
    if (_playMode == PlayMode_Play && elapsedTimer.isValid())
        _timeCtx.setMapSeconds(_playbackStartTime + _timeCtx.getPlaybackRate()
                               * (elapsedTimer.elapsed() / 1000.));
    
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
    double left, top, right, bottom;
    _mapView.getFrustum(left, top, right, bottom);
    double xPixelsToMeters = (right - left) / (double)width();
    double yPixelsToMeters = (top - bottom) / (double)height();
    return MapPoint(pt.x()*xPixelsToMeters, pt.y()*yPixelsToMeters);
}

MapPoint
GLWidget::screenPointToMapPoint(const QPoint &pt)
{
    double left, top, right, bottom;
    _mapView.getFrustum(left, top, right, bottom);
    double xPixelsToMeters = (right - left) / (double)width();
    double yPixelsToMeters = (top - bottom) / (double)height();
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
    _mapView.frameBoundingBox(bbox);
    _updateViewCtx();
    updateGL();
}

void GLWidget::frameLonLatBox(const LonLatBox &llBox)
{
    MapPoint ul = _map->getProjection().toProjection(llBox.upperLeft);
    MapPoint lr = _map->getProjection().toProjection(llBox.lowerRight);
    BoundingBox bbox(ul, lr);
    frameBoundingBox(bbox);
}

void
GLWidget::mouseWheelZoom(float delta)
{
    float size = (delta == 0) ? 1.0 : 1.0 + delta * 0.002;
    zoom(size);
}

void
GLWidget::mouseDragZoom(const QPoint &screenDelta)
{
    float size = width() + height();
    if (size == 0) size = 1.0;
    float diff = (float(screenDelta.x() + screenDelta.y()) * 1000.0) / size;
    mouseWheelZoom(-diff);
}

void
GLWidget::zoom(float amount)
{
    _mapView.zoom(amount);
    _updateViewCtx();
    updateGL();
}

void
GLWidget::moveView(const QPoint &screenDelta)
{
    slotUnlockView();
    MapPoint mapDelta = screenPointToRelativeMapPoint(screenDelta);
    Vec2f moveVector(-mapDelta.x, mapDelta.y);
    _mapView.move(moveVector);
    _updateViewCtx();
    updateGL();
}

LonLatBox
GLWidget::getViewArea() const
{
    double left, top, right, bottom;
    _mapView.getFrustum(left, top, right, bottom);
    MapPoint ul = MapPoint(left, top);
    MapPoint lr = MapPoint(right, bottom);
    LonLat upperLeft = _map->getProjection().fromProjection(ul);
    LonLat lowerRight = _map->getProjection().fromProjection(lr);
    return LonLatBox(upperLeft, lowerRight);
}

void
GLWidget::slotPlay()
{
    _playbackStartTime = _timeCtx.getMapSeconds();
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
    _playbackStartTime = _timeCtx.getMapSeconds();
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

double
GLWidget::getMapSeconds() const
{
    return _timeCtx.getMapSeconds();
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
    if (_playMode == PlayMode_Play) {
        _playbackStartTime = _timeCtx.getMapSeconds();
        elapsedTimer.restart();
    }
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
    double left, top, right, bottom;
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
