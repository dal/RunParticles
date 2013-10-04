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

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    timer.start();
    _camera = CameraOrtho(0.0f, width(), height(), 0.0f, -1, 1);
    _mapView.setCurrentCam(_camera);
    _timeCtx = TimeCtx();
    _map = new Map(&_timeCtx);
    _map->addLayer(new LineLayer());
    MapPoint ctr = _map->getViewCtx()->toProjection(LonLat(37.81155, 122.26667));
    _mapView.recenter(ctr);
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
    lastPos = event->pos();
}

void
GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    
    _mapView.mouseDrag(Vec2i(dx, dy),
                       bool(event->buttons() & Qt::LeftButton),
                       bool(event->buttons() & Qt::MiddleButton),
                       bool(event->buttons() & Qt::RightButton));
    _camera = _mapView.getCamera();
}

void
GLWidget::wheelEvent(QWheelEvent *event)
{
    _mapView.mouseWheel(event->delta());
}

void
GLWidget::update()
{
    _timeCtx.update(timer.elapsed());
    _map->update();
}
