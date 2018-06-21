#ifndef PLAYER_VIDEO_H
#define PLAYER_VIDEO_H

#include <QApplication>
#include <QUrl>
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QFile>
#include <QTimer>
#include <QMutex>

// gst
#include <gst/gst.h>
#include <glib.h>
#include <gst/video/videooverlay.h>

#include "base/base.h"
#include <QObject>
#include <list>

typedef void (*AVProcessor)(unsigned char* data, int len, void * cookie);

class PlayerVideo {
public:
    PlayerVideo(string name);
    ~PlayerVideo();

    void init(AVProcessor video_processor, void * video_processor_cookie, QSize video_resolution);
    void setResolution(QSize video_resolution);
    bool start(int mediaWidth, int mediaHeight, bool isRgb = false);
    void feed(GstBuffer *buffer);

private:
    // 显示
    bool _VideoShow();
    static void  _APPVideoSinkCall(GstElement * elt, void *ptr);
    static gboolean _APPVideoFeedDataCall(void* app_data);
    static void _APPVideoOnNeedDataCall(GstElement *appsrc, guint unused_size, void* app_data);
    static void _APPVideoOnEnoughDataCall(GstElement * pipeline, void *app_data);

private:
    string   _name;
    QMutex * _mutex;

    // video显示回调函数
    AVProcessor _video_processor;
    void *      _video_processor_cookie;

    int         _mediaWidth;
    int         _mediaHeight;
    QSize       _video_resolution;

    GstElement *_app_show_pipeline;

    GstElement *_app_show_sink;
    GstBuffer  *_app_show_buffer = NULL;
    GstElement *_app_video_source = NULL;
    guint       _app_video_src_id = 0;

    friend gboolean _APPVideoFeedDataCall(void* app_data);
    friend void _APPVideoOnEnoughDataCall(GstElement * pipeline, void *app_data);
};

#endif // PLAYER_VIDEO_H
