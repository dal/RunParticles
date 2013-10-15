//
//  GLWidget.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/16/13.
//
//

#include "GLWidget.h"
#include "LineLayer.h"

#include "cinder/gl/gl.h"

GLWidget::GLWidget(Map *map, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    setMouseTracking(true);
    timer.start();
    _map = map;
    // For now center the camera on Oakland
    MapPoint r = _map->getViewCtx()->toProjection(LatLon(37.81155, -122.2));
    MapPoint l = _map->getViewCtx()->toProjection(LatLon(37.81155, -122.3));
    float viewheight = (r.x - l.x) * ((float)height() / (float)width());
    _camera = CameraOrtho(l.x,
                          r.x,
                          r.y - viewheight*0.5,
                          r.y + viewheight*0.5,
                          -1,
                          1);
    _mapView.setCurrentCam(_camera);
    _timeCtx = _map->getTimeCtx();
    _map->addLayer(new LineLayer());
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
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);

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
        _mapView.mouseDrag(Vec2i(-mapDelta.x, -mapDelta.y),
                           bool(event->buttons() & Qt::LeftButton),
                           bool(event->buttons() & Qt::MiddleButton),
                           bool(event->buttons() & Qt::RightButton));
        updateGL();
    }
    _lastPos = eventXy;
    event->accept();
}

void
GLWidget::wheelEvent(QWheelEvent *event)
{
    _mapView.mouseWheel(event->delta());
}

void
GLWidget::update()
{
    _timeCtx->update(timer.elapsed());
    _map->update();
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
