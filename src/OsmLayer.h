#ifndef OSMLAYER_H
#define OSMLAYER_H

#include "Layer.h"
#include "OsmTileSource.h"
#include "Types.h"
#include "ViewCtx.h"

#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/CinderResources.h"
#include "cinder/gl/Texture.h"

#include <unordered_map>

#include <QObject>
#include <QGLShaderProgram>

class OsmLayer : public Layer
{
    
    Q_OBJECT
    
    enum {
        Pass_BaseMap = PassLayer_Background + 1,
        Pass_Count
    } OsmLayerPasses;
    
    
public:
    enum {
        pixelsPerTile = 256
    };
    
    enum {
        numZoomLevels = 18
    };
    
    OsmLayer();
    
    virtual ~OsmLayer();
    
    QString name() const;
    
    QString sport() const;
    
    QDateTime startTime() const;
    
    PassMap passes() const;
    
    void project(const Projection&);
    
    void draw(uint pass, const ViewCtx&, const TimeCtx&);
    
    BoundingBox getBoundingBox() const;
    
    MapPoint position() const;
    
public slots:

    void onTileReady(unsigned int x, unsigned int y, unsigned int z);
    
protected:
    
    struct Tile
    {
        Tile();
        gl::Texture *texture;
        OsmIndex index;
        MapPoint upperLeft;
        MapPoint lowerRight;
        QGLShaderProgram *shader;
        
        void setTexture(const Surface8u &surface);
        
        void draw();
    };
    
    typedef std::unordered_map<OsmIndex, Tile*, OsmHasher<OsmIndex>> TileMap;
    
    OsmTileSource *_tileSource;
    
    TileMap _tiles;
    
    uint _getZoomLevel(double resolution) const;
    
    void _setup();
    
    void _getTileXYAtMapPoint(const MapPoint &pos, int *x, int *y) const;
    
    MapPoint _worldTopLeft, _worldLowerRight;
    
    double _worldSize, _tileSize;
    
    unsigned int _currentZoom, _lastZoom, _numEdgeTiles;
    
    double _lastResolution;
    
    float _resolutions[numZoomLevels];
    
    QGLShaderProgram *_shader;
    
    MapPoint _testTilePos;
    
    int _testTileWidth;
    
    bool _isSetup;
    
};

#endif
