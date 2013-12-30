#ifndef __TRACKLAYER_H__
#define __TRACKLAYER_H__

#include "BoundingBox.h"
#include "Layer.h"
#include "Types.h"

#include "cinder/gl/gl.h"
#include <QString>

class TrackLayer : public Layer
{
public:
    static const Color RunColor, OtherColor, SelectedColor;
    
    TrackLayer(const Track *track);
    
    virtual ~TrackLayer();
    
    QString name() const;
    
    QString sport() const;
    
    QDateTime startTime() const;
    
    unsigned int passes() const { return 2; };
    
    unsigned int duration() const;
    
    void project(const ViewCtx*);
    
    virtual void draw(uint pass, const ViewCtx*, const TimeCtx*);
    
    BoundingBox getBoundingBox() const;
protected:
    
    void _drawPath(const ViewCtx *viewCtx, const TimeCtx *timeCtx);
    
    void _drawParticle(const ViewCtx *viewCtx) const;
    
    const Track *_track;
    
    Path _path_hi, _path_med, _path_lo;
    
    BoundingBox _bounds;
    
    unsigned int _duration;
    
    MapPoint _particlePos;
    
    QDateTime _startTime;
    
    float *_pathBuffer;
};

#endif
