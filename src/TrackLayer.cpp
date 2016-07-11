#include "TrackLayer.h"

#include <QFile>

#include "PathUtil.h"
#include "Projection.h"
#include "Types.h"
#include "ViewCtx.h"

#include <vector>

#define PARTICLE_RADIUS 12.0
#define MEDIUM_LOD_RES 4.0
#define LO_LOD_RES 10.0
#define XLO_LOD_RES 350.0

using namespace cinder;

const Color TrackLayer::SelectedColor = Color( 1, 1, 0 );

const quint32 TrackLayer::_streamHeader = 4;

const quint16 TrackLayer::_classVersion = 1;

bool TrackLayer::_isSetup = false;
gl::DisplayList TrackLayer::_particle;
gl::DisplayList TrackLayer::_selectedParticle;
QGLShaderProgram *TrackLayer::_shader;

void
_loadShader(QGLShader::ShaderType type, const QString &file,
            QGLShaderProgram *program)
{
    QByteArray source;
    QFile qrcFile(file);
    if (qrcFile.open(QIODevice::ReadOnly))
        source = qrcFile.readAll();
    if (source.size()) {
        if (!program->addShaderFromSourceCode(type, source)) {
            qWarning() << "Failed to compile shader '" << file << "':"
            << program->log();
        }
    }
}

void
TrackLayer::_setup()
{
    // Initialize the shaders
    _shader = new QGLShaderProgram();
    _shader->setGeometryInputType(GL_LINES_ADJACENCY_EXT);
    _shader->setGeometryOutputType(GL_TRIANGLE_STRIP);
    _shader->setGeometryOutputVertexCount(7);
    _loadShader(QGLShader::Vertex, ":track1.vert", _shader);
    _loadShader(QGLShader::Fragment, ":track1.frag", _shader);
    _loadShader(QGLShader::Geometry, ":track1.geom", _shader);
    if (!_shader->link())
        qWarning() << "Failed to link shader program";

    _particle = gl::DisplayList(GL_COMPILE);
    _particle.newList();
    gl::drawSolidCircle( Vec2d(0.f, 0.f), 10.);
    gl::color( Color( 0.3, 0.3, 0.3 ) );
    gl::drawStrokedCircle( Vec2d(0.f, 0.f), 10.);
    _particle.endList();
    
    _selectedParticle = gl::DisplayList(GL_COMPILE);
    _selectedParticle.newList();
    gl::color(SelectedColor);
    gl::drawStrokedCircle(Vec2d(0.f, 0.f), 15.);
    gl::drawStrokedCircle(Vec2d(0.f, 0.f), 20.);
    _selectedParticle.endList();
    
    _isSetup = true;
}

TrackLayer::TrackLayer(Track *track) : Layer(),
_particleRadius(PARTICLE_RADIUS),
_mediumLodRes(MEDIUM_LOD_RES),
_loLodRes(LO_LOD_RES),
_xLoLodRes(XLO_LOD_RES),
_track(track),
_duration(0),
_trackColor(Color(1,0,0)),
_trackWidth(1),
_projected(false)
{
    if (!_isSetup)
        _setup();
    int numPts = track->points.size();
    if (numPts > 0)
        _startTime = QDateTime::fromTime_t(track->points[0].time);
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
        } else if ((newPt.pos - lastPt).length() < 4.) {
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

gl::VboMesh
TrackLayer::_makeVbo(const Path &path)
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
    
    // finally, create the mesh
    gl::VboMesh::Layout layout;
    layout.setStaticPositions();
    layout.setStaticIndices();
    layout.setStaticTexCoords2d();
    
    gl::VboMesh vboMesh = gl::VboMesh(vertices.size(), indices.size(), layout,
                           GL_LINES_ADJACENCY_EXT );
    vboMesh.bufferPositions( &(vertices.front()), vertices.size() );
    vboMesh.bufferIndices( indices );
    return vboMesh;
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
    _vboHi = _makeVbo(_path_hi);
    _vboMd = _makeVbo(_path_md);
    _vboLo = _makeVbo(_path_lo);
    _vboXLo = _makeVbo(_path_xlo);
}

void
TrackLayer::draw(uint pass, const ViewCtx &viewCtx, const TimeCtx &timeCtx)
{
    if (!_vboHi)
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
    glPushMatrix();
    glTranslated(w2c.x+_positionOffset.x, w2c.y+_positionOffset.y, 0.);
    bool selected = viewCtx.isSelected(id());
    if (selected)
        gl::color(SelectedColor);
    else
        gl::color(_trackColor);
    if (_vboHi) {
        gl::VboMesh *theMesh = &_vboHi;
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
        _shader->bind();
        float time = timeCtx.getMapSeconds();
        _shader->setUniformValue(_shader->uniformLocation("WIN_SCALE"),
                                 (GLfloat)viewCtx.getViewportWidth(),
                                 (GLfloat)viewCtx.getViewportHeight() );
        _shader->setUniformValue(_shader->uniformLocation("MITER_LIMIT"),
                                 (GLfloat)miter_limit ); /* -1.0 never, 1.0 always */
        _shader->setUniformValue(_shader->uniformLocation("THICKNESS"),
                                 (GLfloat)float(_trackWidth) );
        _shader->setUniformValue(_shader->uniformLocation("TIME_SECONDS"),
                                 (GLfloat)float(time) );
        _shader->setUniformValue(_shader->uniformLocation("SELECTED"),
                                 selected);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        gl::draw(*theMesh);
        _shader->release();
    }
    glPopMatrix();
    
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
    gl::color(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslated(particlePosCamera.x, particlePosCamera.y, 0.0);
    glScalef(radius/10., radius/10., 1.0);
    
    _particle.draw();
    if (viewCtx.isSelected(id())) {
        _selectedParticle.draw();
    }
    glPopMatrix();
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
