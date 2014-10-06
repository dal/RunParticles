#ifndef __TRACKLAYER_H__
#define __TRACKLAYER_H__

#include "BoundingBox.h"
#include "Layer.h"
#include "Types.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/DisplayList.h"
#include <QString>
#include <QOpenGLShaderProgram>

class TrackLayer : public Layer
{
public:
    
    enum TrackLayerPasses {
        Pass_UnselectedPath = PassLayer_MidGround + 1,
        Pass_SelectedPath = PassLayer_MidGround + 4,
        Pass_Particles = PassLayer_Foreground + 1,
        Pass_Count
    };
    
    static const Color RunColor, OtherColor, SelectedColor;
    
    TrackLayer(const Track *track);
    
    virtual ~TrackLayer();
    
    QString name() const;
    
    QString sport() const;
    
    QDateTime startTime() const;
    
    /*
     * TrackLayers require three passes: 
     *  top Draws all the track particles.
     *  mid Draws the path if it is selected
     *  bottom Draws the path if it is not currently selected
     */
    PassMap passes() const;
    
    unsigned int duration() const;
    
    void project(const Projection&);
    
    virtual void draw(uint pass, const ViewCtx&, const TimeCtx&);
    
    BoundingBox getBoundingBox() const;
    
    MapPoint position() const;
    
    bool ephemeral() const;
    
protected:
    
    void _drawPath(const ViewCtx &viewCtx, const TimeCtx &timeCtx);
    
    void _drawParticle(const ViewCtx &viewCtx);
    
    static void _setup();
    
    static float _particleRadius;
    
    float _mediumLodRes;
    
    float _loLodRes;
    
    const Track *_track;
    
    Path _path_hi, _path_med, _path_lo;
    
    BoundingBox _bounds;
    
    unsigned int _duration;
    
    MapPoint _particlePos;
    
    QDateTime _startTime;
    
    float *_pathBuffer;
    
    static bool _isSetup;
    
    static gl::DisplayList _particle, _selectedParticle;
    
};

#endif
