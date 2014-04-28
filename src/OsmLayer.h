#ifndef OSMLAYER_H
#define OSMLAYER_H

#include "Layer.h"
#include "Types.h"
#include "ViewCtx.h"

#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/CinderResources.h"
#include "cinder/gl/Texture.h"

#include <QGLShaderProgram>

class OsmLayer : public Layer
{
    
public:
    enum {
        pixelsPerTile = 256
    };
    
    enum {
        numZoomLevels = 20
    };
    
    OsmLayer();
    
    virtual ~OsmLayer();
    
    QString name() const;
    
    QString sport() const;
    
    QDateTime startTime() const;
    
    void project(const Projection&);
    
    void draw(uint pass, const ViewCtx&, const TimeCtx&);
    
    BoundingBox getBoundingBox() const;
    
    MapPoint position() const;
    
protected:
    uint _getZoomLevel(double resolution) const;
    
    void _setup();
    
    MapPoint _worldTopLeft, _worldLowerRight;
    
    double _worldSize;
    
    unsigned int _currentZoom;
    
    double _lastResolution;
    
    float _resolutions[numZoomLevels];
    
    QGLShaderProgram *_shader;
    
    gl::Texture	*_testTexture;
    
    ImageSourceRef _testImg;
    
    MapPoint _testTilePos;
    
    int _testTileWidth;
    
    bool _isSetup;
    
};

#endif
