#include "GLUtil.h"

#include <QDebug>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <cmath>
#include <vector>

#include "Util.h"

namespace {

const int CircleSegments = 48;

struct Shapes
{
    QOpenGLVertexArrayObject quadVao;
    QOpenGLBuffer quadVbo;
    QOpenGLVertexArrayObject circleVao;
    QOpenGLBuffer circleVbo;
    QOpenGLShaderProgram flatShader;
    QOpenGLContext *context = nullptr;
};

Shapes&
shapes()
{
    static Shapes s;
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (s.context == ctx)
        return s;
    // First use, or the context changed; (re)build the resources.
    s.context = ctx;
    QOpenGLFunctions_4_1_Core *f = GLUtil::gl();

    // Unit quad: x, y, s, t
    const GLfloat quad[] = {
        0.f, 1.f,  0.f, 0.f,
        0.f, 0.f,  0.f, 1.f,
        1.f, 1.f,  1.f, 0.f,
        1.f, 0.f,  1.f, 1.f,
    };
    s.quadVao.destroy();
    s.quadVao.create();
    s.quadVao.bind();
    s.quadVbo.destroy();
    s.quadVbo.create();
    s.quadVbo.bind();
    s.quadVbo.allocate(quad, sizeof(quad));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat),
                             nullptr);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat),
                             reinterpret_cast<void*>(2*sizeof(GLfloat)));
    s.quadVao.release();

    // Circle: a center vertex followed by the rim, closed. It serves both as
    // a triangle fan (filled) and, skipping the center, as a line strip.
    std::vector<GLfloat> circle;
    circle.push_back(0.f);
    circle.push_back(0.f);
    for (int i = 0; i <= CircleSegments; i++) {
        double a = 2. * M_PI * i / CircleSegments;
        circle.push_back(GLfloat(std::cos(a)));
        circle.push_back(GLfloat(std::sin(a)));
    }
    s.circleVao.destroy();
    s.circleVao.create();
    s.circleVao.bind();
    s.circleVbo.destroy();
    s.circleVbo.create();
    s.circleVbo.bind();
    s.circleVbo.allocate(circle.data(), int(circle.size() * sizeof(GLfloat)));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    s.circleVao.release();

    s.flatShader.removeAllShaders();
    Util::loadGLSLShader(QOpenGLShader::Vertex, ":flat.vert", &s.flatShader);
    Util::loadGLSLShader(QOpenGLShader::Fragment, ":flat.frag", &s.flatShader);
    if (!s.flatShader.link())
        qWarning() << "Failed to link flat shader program";
    return s;
}

}

QOpenGLFunctions_4_1_Core*
GLUtil::gl()
{
    static QOpenGLFunctions_4_1_Core funcs;
    static QOpenGLContext *funcsContext = nullptr;
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (ctx != funcsContext) {
        funcs.initializeOpenGLFunctions();
        funcsContext = ctx;
    }
    return &funcs;
}

void
GLUtil::drawUnitQuad()
{
    Shapes &s = shapes();
    s.quadVao.bind();
    gl()->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    s.quadVao.release();
}

void
GLUtil::drawCircle(const QMatrix4x4 &mvp, const QVector4D &color,
                   float radius, bool filled)
{
    Shapes &s = shapes();
    QMatrix4x4 scaled = mvp;
    scaled.scale(radius, radius, 1.0f);
    s.flatShader.bind();
    s.flatShader.setUniformValue("mvp", scaled);
    s.flatShader.setUniformValue("uColor", color);
    s.circleVao.bind();
    if (filled)
        gl()->glDrawArrays(GL_TRIANGLE_FAN, 0, CircleSegments + 2);
    else
        gl()->glDrawArrays(GL_LINE_STRIP, 1, CircleSegments + 1);
    s.circleVao.release();
    s.flatShader.release();
}
