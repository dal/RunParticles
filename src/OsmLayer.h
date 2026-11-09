#ifndef OSMLAYER_H
#define OSMLAYER_H

#include "Layer.h"
#include "OsmTileSource.h"
#include "Types.h"
#include "ViewCtx.h"

#include <memory>
#include <unordered_map>

#include <QImage>
#include <QObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

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
        numZoomLevels = 19
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

    void onTileReady(OsmIndex index);
    
protected:
    
    struct Tile
    {
        Tile();
        
        /* The tile image, uploaded to the GPU on the first draw. */
        QImage image;
        std::unique_ptr<QOpenGLTexture> texture;
        OsmIndex index;
        MapPoint upperLeft;
        MapPoint lowerRight;
        QOpenGLShaderProgram *shader;
        
        void setImage(const QImage &image);
        
        void draw(const ViewCtx &viewCtx);
        
        void _drawQuad(const ViewCtx &viewCtx, const MapPoint &upperLeft,
                       const MapPoint &lowerRight);
    };
    
    typedef std::shared_ptr<Tile> TileRefPtr;
    
    typedef std::unordered_map<OsmIndex, TileRefPtr, OsmHasher<OsmIndex>> TileMap;
    
    OsmTileSource *_tileSource;
    
    TileMap _tiles;
    
    void _drawLabel(const ViewCtx &viewCtx);
    
    
    uint _getZoomLevel(double resolution) const;
    
    void _setup();
    
    void _getTileXYAtMapPoint(const MapPoint &pos,
                              unsigned int *x,
                              unsigned int *y) const;
    
    MapPoint _worldTopLeft, _worldLowerRight;
    
    double _worldSize, _tileSize;
    
    unsigned int _currentZoom, _lastZoom, _numEdgeTiles;
    
    double _lastResolution;
    
    float _resolutions[numZoomLevels];
    
    QOpenGLShaderProgram *_shader;
    
    bool _isSetup;
    
    QOpenGLShaderProgram *_labelShader;
    
    std::unique_ptr<QOpenGLTexture> _labelTexture;
    
    QSizeF _labelSize;
    
    float _labelAscent;
    
};

#endif
