//
//  GLUtil.h
//  RunParticles
//
//  Small helpers for drawing simple shapes with the OpenGL 4.1 core profile.
//  All functions require the GLWidget's context to be current.
//

#ifndef GLUTIL_H
#define GLUTIL_H

#include <QMatrix4x4>
#include <QOpenGLFunctions_4_1_Core>
#include <QVector4D>

namespace GLUtil
{
    /* OpenGL functions for the current context. */
    QOpenGLFunctions_4_1_Core* gl();

    /*
     * Draws the unit square (0,0)-(1,1) with texture coordinates running from
     * (0,0) at the top left to (1,1) at the bottom left. The caller must bind
     * a program that has the "quad.vert" vertex shader.
     */
    void drawUnitQuad();

    /* Draws a circle of the given radius about the origin using a flat color. */
    void drawCircle(const QMatrix4x4 &mvp, const QVector4D &color,
                    float radius, bool filled);
};

#endif
