#ifndef __TRACKLAYER_H__
#define __TRACKLAYER_H__

#include "Layer.h"

#include "BoundingBox.h"
#include "Types.h"

class TrackLayer : public Layer
{
public:
    TrackLayer(const Track *track);
    
    virtual ~TrackLayer();
    
    unsigned int passes() const { return 2; };
    
    unsigned int duration() const;
    
    void project(const ViewCtx*);
    
    virtual void draw(uint pass, const ViewCtx*, const TimeCtx*);
    
    BoundingBox boundingBox() const;
protected:
    
    void _drawPath(const ViewCtx *viewCtx, const TimeCtx *timeCtx);
    
    void _drawParticle(const ViewCtx *viewCtx) const;
    
    const Track *_track;
    
    Path _path_hi, _path_med, _path_lo;
    
    BoundingBox _bounds;
    
    unsigned int _duration;
    
    MapPoint _particlePos;
};

#endif;
