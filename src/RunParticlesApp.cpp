#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include <sqlite3.h>
#include "MapView.h"
#include "Map.h"
#include "TimeCtx.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RunParticlesApp : public AppBasic {
public:
    void prepareSettings(Settings*);
	void setup();
	void mouseDown( MouseEvent event );	
    void mouseDrag( MouseEvent event );
    void mouseWheel( MouseEvent event );
	void update();
	void draw();
private:
    MapView _mapView;
    CameraOrtho _camera;
    double _currentSeconds;
    TimeCtx _timeCtx;
    Map *_map;
};

void RunParticlesApp::prepareSettings( Settings *settings ) {
    settings->setWindowSize(800, 600);
    settings->setFrameRate(60.0f);
}

void RunParticlesApp::setup()
{
    _camera = CameraOrtho(0.0f, getWindowWidth(), getWindowHeight(), 0.0f, -1, 1);
    _mapView.setCurrentCam(_camera);
    _currentSeconds = getElapsedSeconds();
    _timeCtx = TimeCtx();
    _map = new Map(&_timeCtx);
}

void RunParticlesApp::mouseDown( MouseEvent event )
{
    _mapView.mouseDown( event.getPos() );
}

void RunParticlesApp::mouseDrag( MouseEvent event )
{
    _mapView.mouseDrag(event.getPos(), 
                       event.isLeftDown(), 
                       event.isMiddleDown(), 
                       event.isRightDown());
    _camera = _mapView.getCamera();
}

void RunParticlesApp::mouseWheel( MouseEvent event )
{
    _mapView.mouseWheel(event.getWheelIncrement());
    _camera = _mapView.getCamera();
}

void RunParticlesApp::update()
{
    double elapsed = getElapsedSeconds() - _currentSeconds;
    _currentSeconds += elapsed;
    _timeCtx.update(elapsed);
    _map->update();
}

void RunParticlesApp::draw()
{
    // setup camera
	gl::pushMatrices();
	gl::setMatrices( _camera );
    
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color( Color( 1, 0, 0 ) );
    
    gl::drawLine( Vec2f(0, 0), Vec2f(200, 200));
}

CINDER_APP_BASIC( RunParticlesApp, RendererGl )
