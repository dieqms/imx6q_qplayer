#include "paintlabel.h"
#include <QPainter>
#include <QDebug>
#include <QtWidgets/qframe.h>
#include <QWidget>
#include "base/base.h"

PaintLabel::PaintLabel(QWidget *parent):QLabel(parent)
{
    yuvPtr = new unsigned char[1920*1080*4];
    w = 0;
    h = 0;
    format = QImage::Format_RGB16;
}

void PaintLabel::paintEvent(QPaintEvent *event)
{
    QImage * image = new QImage(yuvPtr, w, h, format);

    QPainter paint(this);

    paint.drawImage(0, 0, *image);

//    printf("PaintLabel::paintEvent\n");

    delete image;
}

void PaintLabel::update(unsigned char* data, int len, int w, int h, QImage::Format format)
{
    if (len > 1920*1080*4) {
        delete [] yuvPtr;
        yuvPtr = new unsigned char[len];
    }

    memcpy(yuvPtr, data, len);
    this->w = w;
    this->h = h;
    this->format = format;

    repaint();
}
