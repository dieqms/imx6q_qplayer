#ifndef PAINTLABEL_H
#define PAINTLABEL_H
#include <QLabel>

class PaintLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PaintLabel(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    void update(unsigned char* data, int len, int w, int h, QImage::Format format);

private:
    uchar *yuvPtr;
    int w;
    int h;
    QImage::Format format;
};

#endif // PAINTLABEL_H
