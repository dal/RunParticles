#ifndef __TRACKLAYER_H__
#define __TRACKLAYER_H__

#include "BoundingBox.h"
#include "Layer.h"
#include "Types.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/DisplayList.h"
#include "cinder/gl/Vbo.h"
#include <QDataStream>
#include <QString>
#include <QGLShader>
#include <QGLShaderProgram>

class TrackLayer;

typedef std::shared_ptr<TrackLayer> TrackLayerPtr;

class TrackLayer : public Layer
{
public:
    
    enum TrackLayerPasses {
        Pass_UnselectedPath = PassLayer_MidGround + 1,
        Pass_SelectedPath = PassLayer_MidGround + 4,
        Pass_Particles = PassLayer_Foreground + 1,
        Pass_Count
    };
    
    static const Color SelectedColor;
    
    TrackLayer(const Track *track);
    
    virtual ~TrackLayer();
    
    QString name() const;
    
    QString sourceFilePath() const;
    
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
    
    ColorA getTrackColor() const;
    
    void setTrackColor(const ColorA &color);
    
    unsigned int getTrackWidth() const;
    
    void setTrackWidth(unsigned int width);
    
    const Track* getTrack() const;

protected:
    
    gl::VboMesh _makeVbo(const Path &path);
    
    void _initializeVbos();
    
    void _drawPath(const ViewCtx &viewCtx, const TimeCtx &timeCtx);
    
    void _drawParticle(const ViewCtx &viewCtx);
    
    static void _setup();
    
    float _particleRadius;
    
    float _mediumLodRes;
    
    float _loLodRes;
    
    const Track *_track;
    
    Path _path_hi, _path_md, _path_lo;
    
    BoundingBox _bounds;
    
    unsigned int _duration;
    
    MapPoint _particlePos;
    
    QDateTime _startTime;
    
    ColorA _trackColor;
    
    unsigned int _trackWidth;
    
    float *_pathBuffer;
    
    gl::VboMesh _vboHi, _vboMd, _vboLo;
    
    static bool _isSetup;
    
    static gl::DisplayList _particle, _selectedParticle;
    
    MapPoint _positionOffset;
    
    static QGLShaderProgram *_shader;

private:
    
    static const quint32 _streamHeader;
    
    static const quint16 _classVersion;
    
};

#endif
