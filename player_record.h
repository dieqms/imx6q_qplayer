#ifndef PLAYER_RECORD_H
#define PLAYER_RECORD_H

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

class PlayerRecord {
public:
    PlayerRecord(string name);

    bool start(int mediaWidth, int mediaHeight);
    void feed(GstBuffer *buffer);

private:
    static gboolean _APPRecordFeedDataCall(void* app_data);
    static void _APPRecordOnNeedDataCall(GstElement *appsrc, guint unused_size, void* app_data);
    static void _APPRecordOnEnoughDataCall(GstElement * pipeline, void *app_data);

private:
    string   _name;
    QMutex * _record_mutex;

    int         _mediaWidth;
    int         _mediaHeight;

    GstElement * _app_record_pipeline;

    GstElement *_app_record_source = NULL;
    GstBuffer  *_app_record_buffer = NULL;
    guint       _app_record_src_id = 0;

    friend gboolean _APPRecordFeedDataCall(void* app_data);
    friend void _APPRecordOnNeedDataCall(GstElement *appsrc, guint unused_size, void* app_data);
    friend void _APPRecordOnEnoughDataCall(GstElement * pipeline, void *app_data);
};


#endif // PLAYER_RECORD_H
