#ifndef __TRACKLAYER_H__
#define __TRACKLAYER_H__

#include "BoundingBox.h"
#include "Layer.h"
#include "Types.h"

#include "GLUtil.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDataStream>
#include <QString>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

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
    
    static const ColorA SelectedColor;
    
    TrackLayer(Track *track);
    
    virtual ~TrackLayer();
    
    QString name() const;
    
    void setName(const QString &newName);
    
    QString sourceFilePath() const;
    
    QString sport() const;
    
    void setSport(const QString &newSport);
    
    QDateTime startTime() const;
    
    /*
     * TrackLayers require three passes: 
     *  top Draws all the track particles.
     *  mid Draws the path if it is selected
     *  bottom Draws the path if it is not currently selected
     */
    PassMap passes() const;
    
    unsigned int duration() const;
    
    virtual void project(const Projection&);
    
    virtual void draw(uint pass, const ViewCtx&, const TimeCtx&);
    
    BoundingBox getBoundingBox() const;
    
    MapPoint position() const;
    
    bool ephemeral() const;
    
    bool userEditable() const;
    
    ColorA getTrackColor() const;
    
    void setTrackColor(const ColorA &color);
    
    unsigned int getTrackWidth() const;
    
    void setTrackWidth(unsigned int width);
    
    const Track* getTrack() const;

protected:
    
    /*
     * The GPU buffers for one level of detail of the path. The vertices are
     * drawn as GL_LINES_ADJACENCY.
     */
    struct PathMesh
    {
        QOpenGLVertexArrayObject vao;
        QOpenGLBuffer vertices;
        QOpenGLBuffer indices{QOpenGLBuffer::IndexBuffer};
        int indexCount = 0;
        
        bool valid() const { return indexCount > 0; }
    };
    
    void _makeVbo(const Path &path, PathMesh &mesh);
    
    void _initializeVbos();
    
    void _drawPath(const ViewCtx &viewCtx, const TimeCtx &timeCtx);
    
    void _drawParticle(const ViewCtx &viewCtx);
    
    static void _setup();
    
    float _particleRadius;
    
    float _mediumLodRes;
    
    float _loLodRes;
    
    float _xLoLodRes;
    
    Track *_track;
    
    Path _path_hi, _path_md, _path_lo, _path_xlo;
    
    BoundingBox _bounds;
    
    unsigned int _duration;
    
    MapPoint _particlePos;
    
    QDateTime _startTime;
    
    ColorA _trackColor;
    
    unsigned int _trackWidth;
    
    float *_pathBuffer;
    
    PathMesh _vboHi, _vboMd, _vboLo, _vboXLo;
    
    static bool _isSetup;
    
    
    MapPoint _positionOffset;
    
    static QOpenGLShaderProgram *_shader;

private:
    
    static const quint32 _streamHeader;
    
    static const quint16 _classVersion;
    
    bool _projected;
    
};

#endif
