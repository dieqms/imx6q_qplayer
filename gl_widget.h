#ifndef GL_WIDGET_H
#define GL_WIDGET_H


#include <QFile>
#include <QTimer>
#include <QGLWidget>
#include <QGLShader>
#include <QGLShaderProgram>

class Gl_widget: public QGLWidget
{
    Q_OBJECT
public:
    Gl_widget(int width, int height, const QString& path, QWidget *parent = 0);
    ~Gl_widget();

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);

protected slots:
    void on_timeout();

private:
    int video_width;
    int video_height;

    GLuint y_texture;
    GLuint uv_texture;

    unsigned char* y_data;
    unsigned char* uv_data;

    QFile file;
    QTimer timer;
    QGLShaderProgram program;
};

#endif // GL_WIDGET_H
