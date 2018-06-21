#ifndef GLYUVWIDGET_H
#define GLYUVWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QTimer>

#define ATTRIB_VERTEX 0
#define ATTRIB_TEXTURE 1

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLYuvWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLYuvWidget(QWidget *parent =0);
    ~GLYuvWidget();

    void PlayOneFrame();

public slots:
    void slotShowYuv(uchar *ptr,uint width,uint height); //显示一帧Yuv图像

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

private:
    GLuint textureUniformY; //y纹理数据位置
    GLuint textureUniformU; //u纹理数据位置
    GLuint textureUniformV; //v纹理数据位置

    GLuint idY; //y纹理对象ID
    GLuint idU; //u纹理对象ID
    GLuint idV; //v纹理对象ID

    QOpenGLTexture* textureY;  //y纹理对象
    QOpenGLTexture* textureU;  //u纹理对象
    QOpenGLTexture* textureV;  //v纹理对象

    QOpenGLShader *m_pVSHader;  //顶点着色器程序对象
    QOpenGLShader *m_pFSHader;  //片段着色器对象
    QOpenGLShaderProgram *program; //着色器程序容器

//    QOpenGLBuffer vbo;
    uint videoW;
    uint videoH;
    unsigned char * m_pBufYuv420p = nullptr;
    unsigned char * yuvPtr = nullptr;
};


#endif // GLYUVWIDGET_H
