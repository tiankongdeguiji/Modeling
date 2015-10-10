/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "RenderEngine.h"

RenderEngine::RenderEngine()
    : indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    // 设置平行光的参数
    mDirLight.Ambient  = QVector4D(0.2f, 0.2f, 0.2f, 1.0f);
    mDirLight.Diffuse  = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    mDirLight.Specular = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    mDirLight.Direction = QVector3D(0.0f, 0.0f, -1.0f);
}

RenderEngine::~RenderEngine()
{
    vertexBuffer.destroy();
    indexBuffer.destroy();
    vao.destroy();
}

void RenderEngine::SetProjection(QMatrix4x4 proj) {projection = proj;}
void RenderEngine::SetViewRotation(QQuaternion rot) {rotation = rot;}
void RenderEngine::SetViewTranslation(QVector3D trans) {translation = trans;}
void RenderEngine::SetViewScale(qreal s) {scale = s;}
void RenderEngine::ClearBuffers() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);}

bool RenderEngine::InitRenderEngine()
{
    initializeOpenGLFunctions();

    if (!InitShaders()) return false;

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    InitModel();

    program.setUniformValue("light.Ambient", mDirLight.Ambient);
    program.setUniformValue("light.Diffuse", mDirLight.Diffuse);
    program.setUniformValue("light.Specular", mDirLight.Specular);
    program.setUniformValue("light.Direction", mDirLight.Direction);
    program.setUniformValue("eyePos", 0.0f, 0.0f, 0.0f);

    return true;
}

bool RenderEngine::InitShaders()
{
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl")) return false;
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl")) return false;
    if (!program.link()) return false;
    if (!program.bind()) return false;
    return true;
}

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

void RenderEngine::InitModel()
{
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };

    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    vertexBuffer.create();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer.create();
    indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vao.create();

    vao.bind();

//    // 将double转换为float，int转换为uint增加渲染效率
//    std::vector<QVector3D> vertexs;
//    std::vector<QVector3D> normals;
//    for (int i = 0; i < pn;i++) {
//        vertexs.push_back(QVector3D(mesh->points[i][0], mesh->points[i][1], mesh->points[i][2]));
//        normals.push_back(QVector3D(mesh->normals[i][0], mesh->normals[i][1], mesh->normals[i][2]));
//    }

    vertexBuffer.bind();
    vertexBuffer.allocate(vertices, 24 * sizeof(VertexData));
    indexBuffer.bind();
    indexBuffer.allocate(indices, 34 * sizeof(GLushort));

    program.enableAttributeArray(0);
    program.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(VertexData));

    vao.release();
}

void RenderEngine::DrawModel()
{
    vao.bind();

    QMatrix4x4 modelview;
    modelview.translate(translation);
    modelview.rotate(rotation);
    modelview.scale(scale);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * modelview);
    program.setUniformValue("mv_matrix", modelview);

    program.setUniformValue("mat.Ambient", 7.0/255.0, 174.0/255.0, 235.0/255.0, 1.0);
    program.setUniformValue("mat.Diffuse", 7.0/255.0, 174.0/255.0, 235.0/255.0, 1.0);
    program.setUniformValue("mat.Specular", 0.5f, 0.5f, 0.5f, 16.0);

    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    vao.release();
}
