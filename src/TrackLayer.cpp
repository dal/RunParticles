#include "TrackLayer.h"

#include "PathUtil.h"
#include "Projection.h"
#include "Types.h"
#include "Util.h"
#include "ViewCtx.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QVector4D>

#include <vector>

#define PARTICLE_RADIUS 12.0
#define MEDIUM_LOD_RES 4.0
#define LO_LOD_RES 10.0
#define XLO_LOD_RES 350.0

const ColorA TrackLayer::SelectedColor = ColorA( 1, 1, 0, 1 );

const quint32 TrackLayer::_streamHeader = 4;

const quint16 TrackLayer::_classVersion = 1;

bool TrackLayer::_isSetup = false;
QOpenGLShaderProgram *TrackLayer::_shader;

void
TrackLayer::_setup()
{
    // Initialize the shaders. This needs a current GL context, so it's done
    // on the first draw rather than at construction.
    _shader = new QOpenGLShaderProgram();
    Util::loadGLSLShader(QOpenGLShader::Vertex, ":track1.vert", _shader);
    Util::loadGLSLShader(QOpenGLShader::Geometry, ":track1.geom", _shader);
    Util::loadGLSLShader(QOpenGLShader::Fragment, ":track1.frag", _shader);
    if (!_shader->link())
        qWarning() << "Failed to link shader program:" << _shader->log();
    
    _isSetup = true;
}

TrackLayer::TrackLayer(Track *track) : Layer(),
_particleRadius(PARTICLE_RADIUS),
_mediumLodRes(MEDIUM_LOD_RES),
_loLodRes(LO_LOD_RES),
_xLoLodRes(XLO_LOD_RES),
_track(track),
_duration(0),
_trackColor(ColorA(1, 0, 0, 1)),
_trackWidth(1),
_projected(false)
{
    int numPts = track->points.size();
    if (numPts > 0)
        _startTime = QDateTime::fromSecsSinceEpoch(track->points[0].time);
    if (numPts > 1)
        _duration = track->points[numPts-1].time - track->points[0].time;
    _pathBuffer = (float*)malloc(sizeof(float)*4*numPts);
}

TrackLayer::~TrackLayer() 
{
    free(_pathBuffer);
    _pathBuffer = NULL;
    delete _track;
}

QString
TrackLayer::name() const
{
    return _track->name;
}

void
TrackLayer::setName(const QString &newName)
{
    _track->name = QString(newName);
}

QString
TrackLayer::sourceFilePath() const
{
    return _track->sourceFilePath;
}

QString
TrackLayer::sport() const
{
    return _track->sport;
}

void
TrackLayer::setSport(const QString &newSport)
{
    _track->sport = newSport;
}

QDateTime
TrackLayer::startTime() const
{
    return _startTime;
}

PassMap
TrackLayer::passes() const
{
    PassMap layers;
    layers.insert(Pass_UnselectedPath);
    layers.insert(Pass_SelectedPath);
    layers.insert(Pass_Particles);
    return layers;
}

/*
 * Linear interpolate between a and b by the fractional f.
 */
double lerp(double a, double b, double f)
{
    return a + f * (b - a);
}

MapPoint lerp(const MapPoint& a, const MapPoint& b, double f)
{
    return MapPoint(lerp(a.x, b.x, f), lerp(a.y, b.y, f));
}

unsigned int
TrackLayer::duration() const
{
    return _duration;
}

void
TrackLayer::project(const Projection &projection)
{
    if (_projected)
        return;
    // Project the track into the hi-res path and compute the bounding box
    int startTime = 0;
    MapPoint lastPt;
    for(int i=0; i < _track->points.size(); i++) {
        TrackPoint pt = _track->points[i];
        PathPoint newPt;
        newPt.pos = projection.toProjection(pt.pos);
        if (i == 0) {
            startTime = pt.time;
            _positionOffset = newPt.pos;
            lastPt = newPt.pos;
        } else if (glm::length(newPt.pos - lastPt) < 4.) {
            continue;
        }
        newPt.time = pt.time - startTime;
        _path_hi.push_back(newPt);
        _bounds += newPt.pos;
        lastPt = newPt.pos;
    }
    
    _particleRadius *= projection.getScaleMultiplier();
    
    _mediumLodRes *= projection.getScaleMultiplier();
    _loLodRes *= projection.getScaleMultiplier();
    _xLoLodRes *= projection.getScaleMultiplier();
    _projected = true;
}

void
TrackLayer::_makeVbo(const Path &path, PathMesh &mesh)
{
    std::vector<Vec3f> vertices;
    vertices.reserve( path.size() + 2);
    
    // add an adjacency vertex at the beginning
    vertices.push_back( 2.0f * Vec3f(path[0].pos.x - _positionOffset.x,
                                     path[0].pos.y - _positionOffset.y, 0.)
                       - Vec3f(path[1].pos.x - _positionOffset.x,
                               path[1].pos.y - _positionOffset.y, 0.) );
    
    PathPoint pt;
    // Store the time (seconds) in the z value
    foreach(pt, path) {
        vertices.push_back(Vec3f(pt.pos.x-_positionOffset.x,
                                 pt.pos.y-_positionOffset.y, pt.time));
    }
    
    // add an adjacency vertex at the end
    size_t n = path.size();
    vertices.push_back( 2.0f * Vec3f(path[n-1].pos.x - _positionOffset.x,
                                     path[n-1].pos.y - _positionOffset.y, 0.)
                       - Vec3f(path[n-2].pos.x - _positionOffset.x,
                               path[n-2].pos.y - _positionOffset.y, 0.) );
    
    // now that we have a list of vertices, create the index buffer
    n = vertices.size() - 2;
    std::vector<uint32_t> indices;
    indices.reserve( n * 4 );
    
    for(size_t i=1;i<vertices.size()-2;++i) {
        indices.push_back(i-1);
        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(i+2);
    }
    
    // finally, upload the mesh
    QOpenGLFunctions_4_1_Core *f = GLUtil::gl();
    mesh.vao.create();
    mesh.vao.bind();
    mesh.vertices.create();
    mesh.vertices.bind();
    mesh.vertices.allocate(vertices.data(),
                           int(vertices.size() * sizeof(Vec3f)));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), nullptr);
    mesh.indices.create();
    mesh.indices.bind();
    mesh.indices.allocate(indices.data(),
                          int(indices.size() * sizeof(uint32_t)));
    mesh.indexCount = int(indices.size());
    mesh.vao.release();
}

void
TrackLayer::_initializeVbos()
{
    if (_path_hi.empty() || _path_hi.size() < 2)
        return;
    // Make the medium res path
    _path_md = PathUtil::DouglasPeucker(_path_hi, _mediumLodRes);
    // Make the low res path
    _path_lo = PathUtil::DouglasPeucker(_path_md, _loLodRes*2.);
    _path_xlo = PathUtil::DouglasPeucker(_path_lo, _xLoLodRes);
    _makeVbo(_path_hi, _vboHi);
    _makeVbo(_path_md, _vboMd);
    _makeVbo(_path_lo, _vboLo);
    _makeVbo(_path_xlo, _vboXLo);
}

void
TrackLayer::draw(uint pass, const ViewCtx &viewCtx, const TimeCtx &timeCtx)
{
    if (!_isSetup)
        _setup();
    if (!_vboHi.valid())
        _initializeVbos();
    
    if (!visible() || !_bounds.overlaps(viewCtx.getBoundingBox()))
        return;
    bool selected = viewCtx.isSelected(id());
    switch (pass) {
        case Pass_UnselectedPath:
            if (!selected)
                _drawPath(viewCtx, timeCtx);
            break;
        case Pass_SelectedPath:
            if (selected)
                _drawPath(viewCtx, timeCtx);
            break;
        case Pass_Particles:
            _drawParticle(viewCtx);
            break;
    };
}

void
TrackLayer::_drawPath(const ViewCtx &viewCtx, const TimeCtx &timeCtx)
{
    MapPoint w2c = viewCtx.getWorldToCamera();
    QMatrix4x4 mvp = viewCtx.getProjection();
    mvp.translate(float(w2c.x+_positionOffset.x),
                  float(w2c.y+_positionOffset.y), 0.f);
    bool selected = viewCtx.isSelected(id());
    const ColorA &color = selected ? SelectedColor : _trackColor;
    if (_vboHi.valid()) {
        PathMesh *theMesh = &_vboHi;
        double res = viewCtx.getResolution();
        float miter_limit = 0.75;
        if (res >= _mediumLodRes && res < _loLodRes) {
            theMesh = &_vboMd;
        } else if (res >= _loLodRes && res < _xLoLodRes) {
            theMesh = &_vboLo;
            miter_limit = -0.5;
        } else if (res >= _xLoLodRes) {
            theMesh = &_vboXLo;
            miter_limit = -1.0;
        }
        QOpenGLFunctions_4_1_Core *f = GLUtil::gl();
        _shader->bind();
        float time = timeCtx.getMapSeconds();
        _shader->setUniformValue("mvp", mvp);
        _shader->setUniformValue("uColor",
                                 QVector4D(color.r, color.g, color.b, color.a));
        _shader->setUniformValue("WIN_SCALE",
                                 (GLfloat)viewCtx.getViewportWidth(),
                                 (GLfloat)viewCtx.getViewportHeight() );
        _shader->setUniformValue("MITER_LIMIT",
                                 (GLfloat)miter_limit ); /* -1.0 never, 1.0 always */
        _shader->setUniformValue("THICKNESS", (GLfloat)float(_trackWidth) );
        _shader->setUniformValue("TIME_SECONDS", (GLfloat)float(time) );
        f->glEnable(GL_BLEND);
        f->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        theMesh->vao.bind();
        f->glDrawElements(GL_LINES_ADJACENCY, theMesh->indexCount,
                          GL_UNSIGNED_INT, nullptr);
        theMesh->vao.release();
        _shader->release();
    }
    
    // Choose which path to display
    Path *currentPath = &_path_hi;
    double res = viewCtx.getResolution();
    if (res >= _mediumLodRes && res < _loLodRes)
        currentPath = &_path_md;
    else if (res >= _loLodRes)
        currentPath = &_path_lo;
    
    PathPoint *lastPathPt;
    MapPoint *lastMapPt;
    bool lastInbounds = false;
    for(int i=0; i < currentPath->count(); i++) {
        PathPoint *pt = &(*currentPath)[i];
        MapPoint *thisMapPt = &(pt->pos);
        bool inbounds = viewCtx.getBoundingBox().expand(_particleRadius*2.)
                                                .contains(*thisMapPt);
        if (i == 0 || pt->time < timeCtx.getMapSeconds()) {
            lastMapPt = thisMapPt;
            lastPathPt = pt;
            _particlePos = *lastMapPt;
        } else {
            double elapsed = timeCtx.getMapSeconds() -
                             double(lastPathPt->time);
            int trkElapsed = pt->time - lastPathPt->time;
            double f = (trkElapsed == 0) ? 0. : elapsed / double(trkElapsed);
            if (f > 0.0 && (inbounds || lastInbounds)) {
                MapPoint finalPt = lerp(*lastMapPt, *thisMapPt, f);
                _particlePos = finalPt;
            }
            break;
        }
        lastInbounds = inbounds;
    }
}

void
TrackLayer::_drawParticle(const ViewCtx &viewCtx)
{
    // transform to camera space
    float radius = _particleRadius * viewCtx.getDevicePixelRatio();
    if (radius < viewCtx.getResolution()*2.)
        radius = viewCtx.getResolution()*2.;
    else if (radius / viewCtx.getResolution() > 24.)
        radius = viewCtx.getResolution() * 24.;
    const Vec2d particlePosCamera = viewCtx.getWorldToCamera() + _particlePos;
    QMatrix4x4 mvp = viewCtx.getProjection();
    mvp.translate(float(particlePosCamera.x), float(particlePosCamera.y), 0.f);
    mvp.scale(radius/10., radius/10., 1.0f);
    
    GLUtil::gl()->glEnable(GL_BLEND);
    GLUtil::gl()->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLUtil::drawCircle(mvp, QVector4D(1.f, 1.f, 1.f, 1.f), 10.f, true);
    GLUtil::drawCircle(mvp, QVector4D(0.3f, 0.3f, 0.3f, 1.f), 10.f, false);
    if (viewCtx.isSelected(id())) {
        const QVector4D sel(SelectedColor.r, SelectedColor.g,
                            SelectedColor.b, SelectedColor.a);
        GLUtil::drawCircle(mvp, sel, 15.f, false);
        GLUtil::drawCircle(mvp, sel, 20.f, false);
    }
}

BoundingBox
TrackLayer::getBoundingBox() const
{
    return _bounds;
}

MapPoint
TrackLayer::position() const
{
    return _particlePos;
}

bool
TrackLayer::ephemeral() const
{
    return false;
}

bool
TrackLayer::userEditable() const
{
    return true;
}

ColorA
TrackLayer::getTrackColor() const
{
    return _trackColor;
}

void
TrackLayer::setTrackColor(const ColorA &color)
{
    _trackColor = color;
}

unsigned int
TrackLayer::getTrackWidth() const
{
    return _trackWidth;
}

void
TrackLayer::setTrackWidth(unsigned int width)
{
    _trackWidth = width;
}

const Track*
TrackLayer::getTrack() const
{
    return _track;
}
