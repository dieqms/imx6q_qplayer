#ifndef PLAYER_AUDIO_H
#define PLAYER_AUDIO_H

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

class PlayerAudio
{
public:
    PlayerAudio(string name);
    ~PlayerAudio();

    bool start();
    void feed(GstBuffer *buffer);

private:
    string   _name;
    GstElement *_play_audio_pipeline = NULL;
    GstElement *_audio_sink_buff = NULL;
    GstElement *_audio_src_buff = NULL;
    GstBuffer  *_app_audio_buffer = NULL;
};

#endif // PLAYER_AUDIO_H
