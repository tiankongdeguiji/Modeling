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
#include <cmath>
#include <QtMath>
using namespace std;

RenderEngine::RenderEngine()
    : indexBuffer(QOpenGLBuffer::IndexBuffer),
      indexBuffer2(QOpenGLBuffer::IndexBuffer)
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
void RenderEngine::ClearBuffers() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);}

bool RenderEngine::InitRenderEngine()
{
    initializeOpenGLFunctions();

    if (!InitShaders()) return false;

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    InitModel();
    InitBezierModel();

    // 设置光照参数,  设置视口
    program.setUniformValue("light.Ambient", mDirLight.Ambient);
    program.setUniformValue("light.Diffuse", mDirLight.Diffuse);
    program.setUniformValue("light.Specular", mDirLight.Specular);
    program.setUniformValue("light.Direction", mDirLight.Direction);
    program.setUniformValue("eyePos", 0.0f, 0.0f, 0.0f);

    return true;
}

// 编译和绑定shader
bool RenderEngine::InitShaders()
{
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl")) return false;
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl")) return false;
    if (!program.link()) return false;
    if (!program.bind()) return false;
    return true;
}

// 初始化模型数据
void RenderEngine::InitModel()
{
    EulerOperation euler;
    double p[] = {
        -4.0, -2.0,  0.0,
         4.0, -2.0,  0.0,
         4.0,  2.0,  0.0,
        -4.0,  2.0,  0.0,
        -3.0,  1.0,  0.0,
        -1.0,  1.0,  0.0,
        -1.0, -1.0,  0.0,
        -3.0, -1.0,  0.0,
         1.0,  1.0,  0.0,
         3.0,  1.0,  0.0,
         3.0, -1.0,  0.0,
         1.0, -1.0,  0.0,
    };

//    double p[] = {
//        -3.0,  0.0,  2.0,
//        -4.0,  2.0,  2.0,
//        -4.0, -2.0,  2.0,
//         4.0, -2.0,  2.0,
//        -3.5, -0.5,  2.0,
//        -3.25, -1.25,  2.0,
//        -3.0, -1.5,  2.0,
//        -3.5, -1.5,  2.0,
//    };

    Solid *s;
    Loop *lp;
    HalfEdge *he, *he_kill;
    Vertex *v;
    s = euler.mvfs(v, p);
    lp = s->faces->loops;
    he = euler.mev(v, p+3, lp);
    he = euler.mev(he->endv, p+6, lp);
    he = euler.mev(he->endv, p+9, lp);
    euler.mef(he->endv, v, lp);
    he_kill = he = euler.mev(he->endv, p+12, lp);
    he = euler.mev(he->endv, p+15, lp);
    he = euler.mev(he->endv, p+18, lp);
    he = euler.mev(he->endv, p+21, lp);
    euler.mef(he->endv, he_kill->endv, lp);
    euler.kemr(he_kill->startv, he_kill->endv, lp);
    he_kill = he = euler.mev(he_kill->startv, p+24, lp);
    he = euler.mev(he->endv, p+27, lp);
    he = euler.mev(he->endv, p+30, lp);
    he = euler.mev(he->endv, p+33, lp);
    euler.mef(he->endv, he_kill->endv, lp);
    euler.kemr(he_kill->startv, he_kill->endv, lp);


    double dir[3] = {0,0,-1};
    euler.Sweep(lp->face, dir, 2);
    for(Face *f = s->faces; f != NULL; f = f->next) {
        Triangulate(f, m_vertexs, m_indices);
    }

    // 建立VAO和VBO
    vertexBuffer.create();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer.create();
    indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vao.create();
    // 设置Buffer和制定顶点Attribute
    vao.bind();
    vertexBuffer.bind();
    vertexBuffer.allocate(m_vertexs.data(), m_vertexs.size() * sizeof(QVector3D));
    indexBuffer.bind();
    indexBuffer.allocate(m_indices.data(), m_indices.size() * sizeof(int));
    program.enableAttributeArray(0);
    program.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));
    vao.release();
}

// 绘制模型
void RenderEngine::DrawModel()
{
    vao.bind();

    // 计算模型矩阵
    QMatrix4x4 modelview;
    modelview.translate(translation);
    modelview.rotate(rotation);
    modelview.scale(scale);

    // 设置MVP
    program.setUniformValue("mvp_matrix", projection * modelview);
    program.setUniformValue("mv_matrix", modelview);
    // 设置材质
    program.setUniformValue("mat.Ambient", 7.0/255.0, 174.0/255.0, 235.0/255.0, 1.0);
    program.setUniformValue("mat.Diffuse", 7.0/255.0, 174.0/255.0, 235.0/255.0, 1.0);
    program.setUniformValue("mat.Specular", 0.3f, 0.3f, 0.3f, 16.0);

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    vao.release();
}

// Limited:控制顶点在曲线一侧
void RenderEngine::InitBezierModel() {
    EulerOperation euler;
    double p[] = {
        -4.0, -2.0,  0.0,
         4.0, -2.0,  0.0,
         4.0,  2.0,  0.0,
        -4.0,  2.0,  0.0,
        -3.0,  1.0,  0.0,
        -1.0,  1.0,  0.0,
        -1.0, -1.0,  0.0,
        -3.0, -1.0,  0.0,
         1.0,  1.0,  0.0,
         3.0,  1.0,  0.0,
         3.0, -1.0,  0.0,
         1.0, -1.0,  0.0,
    };
//    double p[] = {
//         0.0, -2.0,  0.0,
//         2.0,  0.0,  0.0,
//         0.0,  2.0,  0.0,
//        -2.0,  0.0,  0.0,
//    };

    Solid *s;
    Loop *lp;
    HalfEdge *he, *he_kill;
    Vertex *v;
    s = euler.mvfs(v, p);
    lp = s->faces->loops;
    he = euler.mev(v, p+3, lp);
    he = euler.mev(he->endv, p+6, lp);
    he = euler.mev(he->endv, p+9, lp);
    euler.mef(he->endv, v, lp);
    he_kill = he = euler.mev(he->endv, p+12, lp);
    he = euler.mev(he->endv, p+15, lp);
    he = euler.mev(he->endv, p+18, lp);
    he = euler.mev(he->endv, p+21, lp);
    euler.mef(he->endv, he_kill->endv, lp);
    euler.kemr(he_kill->startv, he_kill->endv, lp);
    he_kill = he = euler.mev(he_kill->startv, p+24, lp);
    he = euler.mev(he->endv, p+27, lp);
    he = euler.mev(he->endv, p+30, lp);
    he = euler.mev(he->endv, p+33, lp);
    euler.mef(he->endv, he_kill->endv, lp);
    euler.kemr(he_kill->startv, he_kill->endv, lp);

    vector<Point3D> path;
//    path.push_back(Point3D(0,0,0));
//    path.push_back(Point3D(0,0,-10));
//    path.push_back(Point3D(0,-10,-10));
//    path.push_back(Point3D(0,-10,0));
//    path.push_back(Point3D(-10,-10,0));
    path.push_back(Point3D(0,0,0));
    path.push_back(Point3D(0,0,-10));
    path.push_back(Point3D(0,-10,0));
    path.push_back(Point3D(0,-10,-10));
    euler.BezierSweep(lp->face, path);
    for(Face *f = s->faces; f != NULL; f = f->next) {
        Triangulate(f, m_vertexs2, m_indices2);
    }

    // 建立VAO和VBO
    vertexBuffer2.create();
    vertexBuffer2.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer2.create();
    indexBuffer2.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vao2.create();
    // 设置Buffer和制定顶点Attribute
    vao2.bind();
    vertexBuffer2.bind();
    vertexBuffer2.allocate(m_vertexs2.data(), m_vertexs2.size() * sizeof(QVector3D));
    indexBuffer2.bind();
    indexBuffer2.allocate(m_indices2.data(), m_indices2.size() * sizeof(int));
    program.enableAttributeArray(0);
    program.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));
    vao2.release();
}

void RenderEngine::DrawBezierModel()
{
    vao2.bind();

    // 计算模型矩阵
    QMatrix4x4 modelview;
    modelview.translate(translation);
    modelview.rotate(rotation);
    modelview.scale(scale);

    // 设置MVP
    program.setUniformValue("mvp_matrix", projection * modelview);
    program.setUniformValue("mv_matrix", modelview);
    // 设置材质
    program.setUniformValue("mat.Ambient", 7.0/255.0, 174.0/255.0, 235.0/255.0, 1.0);
    program.setUniformValue("mat.Diffuse", 7.0/255.0, 174.0/255.0, 235.0/255.0, 1.0);
    program.setUniformValue("mat.Specular", 0.3f, 0.3f, 0.3f, 16.0);

    glDrawElements(GL_TRIANGLES, m_indices2.size(), GL_UNSIGNED_INT, 0);

    vao2.release();
}

// 功能：带洞的面片三角化
// 参数：f-Brep结构的面，vertexs-物体的顶点数组(返回), indices-物体的顶点索引(返回)
bool RenderEngine::Triangulate(Face *f, vector<QVector3D> &vertexs, vector<int> &indices)
{
    Loop *lp = f->loops;
    HalfEdge *he = lp->halfedges;
    Vertex *v = he->startv;
    double eps = 10e-6;
    
    // 计算面法向
    HalfEdge *he_t = he;
    double diff[3];
    // 寻找外环上一个凸点(x坐标最大)所属的半边
    while(he_t != lp->halfedges) {
        he_t = he_t->next;
        diff[0] = he_t->startv->vcoord[0] - he->startv->vcoord[0];
        diff[1] = he_t->startv->vcoord[1] - he->startv->vcoord[1];
        diff[2] = he_t->startv->vcoord[2] - he->startv->vcoord[2];
        if(diff[0] > eps || fabs(diff[0]) < eps && diff[1] > eps
                || fabs(diff[0]) < eps && fabs(diff[1]) < eps && diff[2] > eps)
            he = he_t;
    }
    v = he->prev->startv;
    QVector3D A = QVector3D(v->vcoord[0], v->vcoord[1], v->vcoord[2]);
    v = he->startv;
    QVector3D B = QVector3D(v->vcoord[0], v->vcoord[1], v->vcoord[2]);
    v = he->next->startv;
    QVector3D C = QVector3D(v->vcoord[0], v->vcoord[1], v->vcoord[2]);
    QVector3D n = QVector3D::crossProduct(B-A, C-B).normalized();

    // 计算旋转到xy平面上的四元数
    float angle = acos(min(1.0f, QVector3D::dotProduct(n,QVector3D(0,0,1))));
    QVector3D rotAxis = QVector3D::crossProduct(n,QVector3D(0,0,1));
    if(rotAxis.length() < eps) rotAxis = QVector3D(1,0,0);    // 法向与z轴共线，自定义旋转轴
    QQuaternion rotation = QQuaternion::fromAxisAndAngle(rotAxis,qRadiansToDegrees(angle));

    // 将面投影到二维平面上
    vector<vector<QVector2D> > face_2d; // 投影的二维面
    QVector3D p;
    int v_num = vertexs.size();         // 缓存之前顶点个数
    int index;
    while(lp != NULL) {
        vector<QVector2D> loop_2d;
        index = 0;
        he = lp->halfedges;
        v = he->startv;
        while(true) {
            p = QVector3D(v->vcoord[0], v->vcoord[1], v->vcoord[2]);
            vertexs.push_back(p);
            p = rotation * p;
            loop_2d.push_back(QVector2D(p[0], p[1]));
            v = (he = he->next)->startv;
            index++;
            if (he == lp->halfedges) break;
        }
        face_2d.push_back(loop_2d);
        lp = lp->next;     
    }

    // 三角剖分
    list<TPPLPoly> input;
    bool inner_flag = false;
    for(int i = 0; i < face_2d.size(); i++) {
        TPPLPoly p;
        vector<QVector2D> loop_2d = face_2d[i];
        p.Init(loop_2d.size());
        if(inner_flag) {
            int a = 0;
        }
        for(int j = 0; j < loop_2d.size(); j++) {
            p[j].x = loop_2d[j][0];
            p[j].y = loop_2d[j][1];
            p.SetHole(inner_flag);         
        }
        input.push_back(p);
        if(!inner_flag) inner_flag = true;
    }
    list<TPPLPoly> output;
    int res = TPPLPartition().Triangulate_EC(&input, &output);
    if (res != 1) return false;

    // 输出返回顶点索引
    QVector2D ptemp;
    int offset;
    bool find = false;
    for (list<TPPLPoly>::iterator poly = output.begin(); poly != output.end(); poly++) {
        for (int i = 0; i < 3; i++) {
            offset = 0;
            find = false;
            ptemp[0]=(*poly)[i].x;
            ptemp[1]=(*poly)[i].y;
            for(int j = 0; j < face_2d.size(); j++) {
                if (find) break;
                vector<QVector2D> loop_2d = face_2d[j];
                for(int k = 0; k < loop_2d.size(); k++) {
                    if((ptemp - loop_2d[k]).length() < eps) {
                        indices.push_back(v_num + offset + k);
                        find = true;
                        break;
                    }
                }
                offset += loop_2d.size();
            }
        }
    }
    return true;
}
