/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
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

#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include "base/base.h"

GLWidget::GLWidget(QWidget *parent, QString name)
    : QOpenGLWidget(parent),
      clearColor(Qt::black),
      xRot(0),
      yRot(0),
      zRot(0),
      program(NULL),
      _imageData(NULL),
      _imageDataLen(0)
{
    setUpdateBehavior(QOpenGLWidget::PartialUpdate);

    _name = name;
    textures = 0;
    _imageUpdated = false;

    _mutex = new QMutex();

    _updateTimer = new QTimer();
    connect(_updateTimer, SIGNAL(timeout()), this, SLOT(updateWidgetTimer()));
    _updateTimer->start(10);
}

void GLWidget::updateWidgetTimer()
{
    //    bool need_update = false;
    ////    _mutex->lock();
    //    need_update = _imageUpdated;
    //    _imageUpdated = false;
    ////    _mutex->unlock();

    //    long long old = Base::TimeUtil::NowMillSeconds();

    //    _mutex->lock();
    if (_imageUpdated) {

        _imageUpdated = false;
        update();
        //        repaint();
        //        printf("###Player [%s] update used %ld ms\n", _name.toStdString().c_str(), Base::TimeUtil::NowMillSeconds() - old);

    }
    //    _mutex->unlock();

    //        printf("###Player [%s] update @ %lld ms\n", _name.toStdString().c_str(), old);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    delete textures;
    delete program;
    doneCurrent();

    delete _mutex;

    _updateTimer->stop();
    delete _updateTimer;

    if (_imageData) {
        free(_imageData);
    }
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(200, 200);
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
    xRot += xAngle;
    yRot += yAngle;
    zRot += zAngle;
    update();
}

void GLWidget::setClearColor(const QColor &color)
{
    clearColor = color;
    update();
}

void GLWidget::updateWidget(uchar *ptr, int len, uint width, uint height, QImage::Format format)
{
    //    long long now = Base::TimeUtil::NowMillSeconds();
    //    printf("Player [%s] updateWidget enter @ %lld ms, %d\n", _name.toStdString().c_str(), now, len);

    //    _mutex->lock();
    if (_imageUpdated) {
        printf("###Player [%s] image have not shown\n", _name.toStdString().c_str());
        //        _mutex->unlock();
        return;
    }
    //    _mutex->unlock();

    _imageFormat = format;
    _imageW = width;
    _imageH = height;

    if (_imageData != NULL) {
        if (this->_imageDataLen < len) {
            printf("GLWidget updateWidget size change from %d to %d\n", this->_imageDataLen, len);

            this->_imageDataLen = len;
            free(_imageData);
            _imageData = (uchar*)malloc(this->_imageDataLen);
        }
    }
    else {
        this->_imageDataLen = len;
        _imageData = (uchar*)malloc(this->_imageDataLen);
    }

    memcpy(_imageData, ptr, len);

    //    _mutex->lock();
    _imageUpdated = true;
    //    _mutex->unlock();

    //    printf("Player [%s] feedImage exit\n", _name.toStdString().c_str());
}

void GLWidget::initializeGL()
{
    _imageData = (uchar*)malloc(1920 * 1080 * 3 / 2);
    this->_imageDataLen = 1920 * 1080 * 3 / 2;

    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);

    makeObject();

#if 0
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
            "attribute highp vec4 vertex;\n"
            "attribute mediump vec4 texCoord;\n"
            "varying mediump vec4 texc;\n"
            "uniform mediump mat4 matrix;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = matrix * vertex;\n"
            "    texc = texCoord;\n"
            "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
            "uniform sampler2D texture;\n"
            "varying mediump vec4 texc;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = texture2D(texture, texc.st);\n"
            "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();
    program->bind();
    program->setUniformValue("texture", 0);

#if 0
    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);
    m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);

    program->setUniformValue("matrix", m);
    //    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    //    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    //    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    //    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
#else
    QMatrix4x4 m;
    m.ortho(-1.0f, +1.0f, +1.0f, -1.0f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);
    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));
#endif

#else
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
            "attribute vec3 pos;\n"
            "attribute highp vec4 vertex;\n"
            "attribute mediump vec4 texCoord;\n"
            "varying mediump vec4 texc;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    texc = texCoord;\n"
            "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
            "uniform sampler2D texture;\n"
            "varying mediump vec4 texc;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = texture2D(texture, texc.st);\n"
            "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();
    program->bind();
    program->setUniformValue("texture", 0);

    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

    textureUniformTexture = program->uniformLocation("texture");
#endif
}

void GLWidget::paintGL()
{
    //    printf("Player [%s] paintGL enter\n", _name.toStdString().c_str());
    //    _mutex->lock();
    if (!_imageData) {
        //        _mutex->unlock();
        return;
    }
    //    _mutex->unlock();

    if (textures == NULL) {
        //        QImage *image = new QImage(_imageData, _imageW, _imageH, _imageFormat);
        //        textures = new QOpenGLTexture(*image);
        //        textures->setMinificationFilter(QOpenGLTexture::Nearest);
        //        textures->setMagnificationFilter(QOpenGLTexture::Linear);
        //        textures->setWrapMode(QOpenGLTexture::Repeat);
        //        delete image;

        textures = new QOpenGLTexture(QOpenGLTexture::Target2D);
        textures->create();
        textureId = textures->textureId();

        //        textures->setFormat(QOpenGLTexture::R5G6B5);
        //        textures->setSize(_imageW, _imageH);
        //        textures->allocateStorage();
        //        //        textures->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5);
        //        textures->setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5, _imageData);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageW, _imageH, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, _imageData);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glUniform1i(textureUniformTexture, 0);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageW, _imageH, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, _imageData);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glUniform1i(textureUniformTexture, 0);
        //        textures->destroy();
        //        textures->create();

        //        textures->setFormat(QOpenGLTexture::R5G6B5);
        //        textures->setSize(_imageW, _imageH);
        //        textures->allocateStorage();
        //        //        textures->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5);
        //        textures->setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5, _imageData);
    }

    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    if (textures) {
    //        if (!textures->isBound())
    //            textures->bind();
    //    }

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //    printf("Player [%s] paintGL exit\n", _name.toStdString().c_str());
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    //    int side = qMin(width, height);
    //    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    //    int dx = event->x() - lastPos.x();
    //    int dy = event->y() - lastPos.y();

    //    if (event->buttons() & Qt::LeftButton) {
    //        rotateBy(8 * dy, 8 * dx, 0);
    //    } else if (event->buttons() & Qt::RightButton) {
    //        rotateBy(8 * dy, 0, 8 * dx);
    //    }
    //    lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

//void GLWidget::update()
//{
//    if (_imageUpdated) {
//        QWidget::update();
//        _imageUpdated = false;
//    }
//}

void GLWidget::makeObject()
{
    static const GLfloat vertices[]{
        //顶点坐标
        -1.0f,-1.0f,
        -1.0f,+1.0f,
        +1.0f,+1.0f,
        +1.0f,-1.0f,
        //纹理坐标
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(vertices,sizeof(vertices));
}
