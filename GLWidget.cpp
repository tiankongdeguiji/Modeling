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

#include "GLWidget.h"

#include <QMouseEvent>

#include <QtMath>
#include <cmath>

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    renderer(0),
    scrollDelta(0)
{
    rotation = QQuaternion::fromAxisAndAngle(QVector3D(1, 1, 1), -30);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    delete renderer;
    doneCurrent();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    // 计算缩放量
    scrollDelta +=  e->delta() / 1200.0;
    if(scrollDelta > 0.9) scrollDelta = 0.9;
    update();
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    oldMousePos = QVector2D(e->localPos());
    newMousePos = QVector2D(e->localPos());
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
     newMousePos = QVector2D(e->localPos());
     // 左键控制旋转，计算新的rotation
     if(e->buttons() & Qt::LeftButton) updateRotation();
     // 中间控制拖动，计算新的translation
     if(e->buttons() & Qt::MidButton) translation += (newMousePos - oldMousePos) / radioScreenToView;
     oldMousePos = QVector2D(e->localPos());
     update();
}

void GLWidget::updateRotation()
{
    QVector3D v = getArcBallVector(oldMousePos);
    QVector3D u = getArcBallVector(newMousePos);

    // 根据算得的轨迹球上的两个向量计算旋转轴和旋转角度
    float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(u,v)));
    QVector3D rotAxis = QVector3D::crossProduct(v,u);
//    QMatrix4x4 eye2ObjSpaceMat = rotationMat.inverted();
//    QVector3D objSpaceRotAxis = eye2ObjSpaceMat * rotAxis;
    // 计算新的rotation（四元数）
    rotation = QQuaternion::fromAxisAndAngle(rotAxis,qRadiansToDegrees(angle))*rotation;
}

QVector3D GLWidget::getArcBallVector(QVector2D mousePos)
{
   // 计算屏幕坐标在轨迹球上的xy坐标
   QVector3D pt = QVector3D(2.0 * mousePos.x() / GLWidget::width() - 1.0, 2.0 * mousePos.y() / GLWidget::height() - 1.0 , 0);
   pt.setY(pt.y() * -1);

   // 计算在轨迹球上的z轴坐标
   float xySquared = pt.x() * pt.x() + pt.y() * pt.y();
   if(xySquared <= 1.0)
       pt.setZ(std::sqrt(1.0 - xySquared));
   else
       pt.normalize();
   return pt;
}

void GLWidget::initializeGL()
{
    renderer = new RenderEngine;
    if(!renderer->InitRenderEngine()) close();
}

void GLWidget::resizeGL(int w, int h)
{
    // 计算屏幕宽高比
    aspect = qreal(w) / qreal(h ? h : 1);
    // 屏幕坐标和视口坐标的比例
    radioScreenToView = h / 10.0;

    const qreal zNear = -100.0, zFar = 100.0;
    // 计算projection
    projection.setToIdentity();  
    projection.ortho(-5.0*aspect, 5.0*aspect, -5.0, 5.0, zNear, zFar);
}

void GLWidget::paintGL()
{
    renderer->SetProjection(projection);
    renderer->SetViewRotation(rotation);
    renderer->SetViewScale(1 - scrollDelta);
    renderer->SetViewTranslation(QVector3D(translation.x(), -translation.y(), -5.0));

    renderer->ClearBuffers();  
    renderer->DrawModel();
}
