#ifndef OSMLAYER_H
#define OSMLAYER_H

#include "Layer.h"
#include "ViewCtx.h"

class OsmLayer : public Layer
{
    
public:
    enum {
        pixelsPerTile = 256
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
    
    MapPoint _worldTopLeft, _worldLowerRight;
    
    double _worldSize;
    
    unsigned int _currentZoom;
    
    double _lastResolution;
    
};

#endif
