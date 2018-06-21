#ifndef GPLAYER_H
#define GPLAYER_H

#include <QApplication>
#include <QUrl>
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QFile>
#include <QTimer>
#include <QMutex>

#include "player_record.h"
#include "player_video.h"
#include "player_audio.h"

using std::string;
using std::list;
using std::map;

using Base::Pthread;
using Base::PthreadMutex;

using namespace std;

class Cache : public QObject, public Pthread {
    Q_OBJECT

public:
    Cache(AVProcessor processor, void * cookie);

    void push(char *data, size_t len);
    void push(string data);

private:
    void runner(void * var);

    PthreadMutex        _mutex;
    list<std::string>   _cache;

public:
    AVProcessor _processor;
    void *      _cookie;

signals:
    void newData(unsigned char *data, int len);
};


class PlayerCore : public Pthread {
public:
    enum MediaType{
        I420,
        MP4,
        RTP,
        RTSP
    };

    enum AudioMute{
        Mute,
        UnMute
    };

    enum RecordOnOrOff{
        RecordOn,
        RecordOff
    };

    struct MediaMetaData {
        string file_path;
        MediaType type;
        QSize resolution;
        int port;
        string sdp;
    };

    PlayerCore(string name);
    ~PlayerCore();

    bool Init(int argc, char *argv[], AVProcessor video_processor, void * video_processor_cookie, QSize video_resolution);
    void SetResolution(QSize resolution);

    bool Play(MediaMetaData media, AudioMute mute = UnMute, RecordOnOrOff record = RecordOff);
    void Stop();
    void Seek(int64_t pos = 0);

//    /* Attempt to seek to the given percentage through the file */
//    void seek_to(gdouble percentage)
//    {
//        GstFormat fmt = GST_FORMAT_TIME;
//        gint64 length;

//        /* If it seems safe to attempt a seek... */
//        if (play && gst_element_query_duration(play, &fmt, &length)) {
//            /* ...calculate where to seek to */
//            gint64 target = ((gdouble)length * (percentage / 100.0));

//            /* ...and attempt the seek */
//            if (!gst_element_seek(play, 1.0, GST_FORMAT_TIME,
//                GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,
//                target, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
//            g_print("Failed to seek to desired position\n");
//        }
//    }

protected:
    bool _DecodeMp4(const char* file, AudioMute mute = Mute);
    bool _DecodeYUV(const char* file, int mediaWidth, int mediaHeight);
    bool _DecodeRTP(int port, const char* sdp);
    bool _DecodeRTSP(const char* uri);

    // 解码
    static gboolean _APPRawPipeBusCall(GstBus *bus, GstMessage *msg, gpointer data);
    static void  _APPVideoSinkCall(GstElement * elt, void *ptr);
    static void  _APPAudioSinkCall(GstElement * elt, void *ptr);

    void runner(void * var);

    static void _GstLooper(void * arg);

private:
    string   _name;
    QMutex * _mutex;

    Cache * _video_cacher;
    QSize   _video_resolution;

    // 录制
    PlayerRecord* _recorder;
    PlayerVideo * _video;
    PlayerAudio * _audio;

    // gst pipeline
    GstElement * _video_pipeline;
    GstElement * _audio_pipeline;

    GstElement * _video_sink_buff;
    GstElement * _audio_sink_buff;

    GstBuffer  * _app_audio_buffer = NULL;

    // gloop消息循环
    static bool  _gst_inited;
    static GMainLoop * _loop;
    static Pthread * _gst_loop_thread;

    friend void  _APPVideoSinkCall(GstElement * elt, void *ptr);
    friend void  _APPAudioSinkCall(GstElement * elt, void *ptr);
    friend gboolean _APPRawPipeBusCall(GstBus *bus, GstMessage *msg, gpointer data);
    friend void _GstLooper(void * arg);
};

#endif // GPLAYER_H
