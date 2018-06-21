#include "player_video.h"


PlayerVideo::PlayerVideo(string name)
{
    _name = name;
    _mutex = new QMutex();
    _mutex = new QMutex();
//    _video_sink_buff = NULL;
    _app_show_pipeline = NULL;
    _app_show_buffer = NULL;
    _video_processor = NULL;
    _video_processor_cookie = NULL;
    _mediaWidth = -1;
    _mediaHeight = -1;
}

PlayerVideo::~PlayerVideo()
{
    if (_app_show_pipeline) {
        gst_element_set_state (_app_show_pipeline, GST_STATE_NULL);
        gst_object_unref (_app_show_pipeline);
        _app_show_pipeline = NULL;
    }
}

/* called when the appsink notifies us that there is a new buffer ready for
* processing */
void PlayerVideo::_APPVideoSinkCall(GstElement * elt, void *ptr)
{
    PlayerVideo * gplayer = (PlayerVideo*)ptr;

    GstBuffer *buffer;
    GstMapInfo map;
    GstSample *sample;

    /* get the buffer from appsink */
    g_signal_emit_by_name(gplayer->_app_show_sink, "pull-sample", &sample, NULL);
    if (sample) {
        memset(&map, 0, sizeof(GstMapInfo));
        buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        //        printf("\n sample got of size = %d, width = %d, height = %d\n", map.size, width, height);
        //        gplayer->_video_cacher->push((char *)map.data, map.size);
        if (gplayer->_video_processor)
            gplayer->_video_processor((unsigned char *)map.data, map.size, gplayer->_video_processor_cookie);

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
    }
}

gboolean PlayerVideo::_APPVideoFeedDataCall(void* app_data)
{
    PlayerVideo * gplayer = (PlayerVideo*)app_data;
    //    printf("PlayerVideo %s feed data!\n", gplayer->_name.c_str());

    gplayer->_mutex->lock();
    if (gplayer->_app_show_pipeline && gplayer->_app_show_buffer) {
        GstFlowReturn ret;
        g_signal_emit_by_name(gplayer->_app_video_source, "push-buffer", gplayer->_app_show_buffer, &ret );
        if (ret != GST_FLOW_OK) {
            /* some error, stop sending data */
            printf("### PlayerVideo %s fail feed data!\n", gplayer->_name.c_str());
        }

        gst_buffer_unref(gplayer->_app_show_buffer);
        gplayer->_app_show_buffer = NULL;
    }
    gplayer->_mutex->unlock();

    usleep(20);

    return TRUE;
}

void PlayerVideo::_APPVideoOnNeedDataCall(GstElement *appsrc,
                                         guint unused_size,
                                         void* app_data)
{
    PlayerVideo * gplayer = (PlayerVideo*)app_data;
    //    printf("PlayerVideo %s need data!\n", gplayer->_name.c_str());
    if (gplayer->_app_video_src_id == 0) {
        gplayer->_app_video_src_id = g_idle_add ((GSourceFunc) _APPVideoFeedDataCall, app_data);
    }

    usleep(10);

    return;
}

void PlayerVideo::_APPVideoOnEnoughDataCall(GstElement * pipeline, void *app_data)
{
    PlayerVideo * gplayer = (PlayerVideo*)app_data;
    printf("###PlayerVideo %s enough data!\n", gplayer->_name.c_str());
    if (gplayer->_app_video_src_id != 0) {
        g_source_remove (gplayer->_app_video_src_id);
        gplayer->_app_video_src_id = 0;
    }

    usleep(10);
    return;
}

void PlayerVideo::init(AVProcessor video_processor, void * video_processor_cookie, QSize video_resolution)
{
    _video_resolution = video_resolution;
    _video_processor = video_processor;
    _video_processor_cookie = video_processor_cookie;
}

void PlayerVideo::setResolution(QSize video_resolution)
{
    _video_resolution = video_resolution;

    start(_mediaWidth, _mediaHeight);
}

bool PlayerVideo::start(int mediaWidth, int mediaHeight, bool isRgb)
{
    _mediaWidth = mediaWidth;
    _mediaHeight = mediaHeight;

    if (_mediaWidth == -1 || _mediaHeight == -1)
        return false;

    _mutex->lock();
    if (_app_show_pipeline) {
        gst_element_set_state (_app_show_pipeline, GST_STATE_NULL);
        gst_object_unref (_app_show_pipeline);
        _app_show_pipeline = NULL;
    }

    char cmd[256] = {0};
    GstBus *bus = NULL;

    if (isRgb) {
        /* Prepare audio play pipe */
        sprintf(cmd, "appsrc name=testsource ! videoparse width=%d height=%d format=29 ! videorate max-rate=20 ! imxvideoconvert_g2d rotation=0 in-place=true ! video/x-raw, format=RGB16, width=%d, height=%d ! appsink name = showsink sync=false ",
                _mediaWidth, _mediaHeight, _video_resolution.width(), _video_resolution.height());
    }
    else {
        sprintf(cmd, "appsrc name=testsource ! videoparse width=%d height=%d ! videorate max-rate=20 ! imxvideoconvert_g2d rotation=0 in-place=true ! video/x-raw, format=RGB16, width=%d, height=%d ! appsink name = showsink sync=false ",
                _mediaWidth, _mediaHeight, _video_resolution.width(), _video_resolution.height());
    }

    printf("PlayerVideo [VIDEO] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _app_show_pipeline = gst_parse_launch(cmd, NULL);
    if(!_app_show_pipeline){
        _mutex->unlock();
        return false;
    }

    //        _audio_src_buff = gst_bin_get_by_name (GST_BIN (_show_pipeline), "testsource");
    //        /* configure for time-based format */
    //        g_object_set (_audio_src_buff, "format", GST_FORMAT_TIME, NULL);
    //        /* uncomment the next line to block when appsrc has buffered enough */
    //        g_object_set (_audio_src_buff, "block", TRUE, NULL);
    //        gst_object_unref (_audio_src_buff);

    _app_video_source = gst_bin_get_by_name (GST_BIN(_app_show_pipeline), "testsource");
    if(!_app_video_source){
        printf("[ERROR] GPlayer gst_bin_get_by_name fail!\n");
        _mutex->unlock();
        return false;
    }

    /* get sink */
    _app_show_sink = gst_bin_get_by_name(GST_BIN(_app_show_pipeline), "showsink");
    if(!_app_show_sink){
        printf("[ERROR] GPlayer gst_bin_get_by_name fail!\n");
        _mutex->unlock();
        return false;
    }

#ifndef VIDEO_PULL
    GstElement *source = gst_bin_get_by_name (GST_BIN (_app_show_pipeline), "testsource");
    g_signal_connect(source, "need-data", G_CALLBACK (_APPVideoOnNeedDataCall), this);
    g_signal_connect(source, "enough-data", G_CALLBACK(_APPVideoOnEnoughDataCall), this);
    gst_object_unref(source);
#endif

    g_object_set(G_OBJECT(_app_show_sink), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect(_app_show_sink, "new-sample", G_CALLBACK(_APPVideoSinkCall), this);

    //        bus = gst_element_get_bus (_app_show_pipeline);
    //        gst_bus_add_watch (bus, (GstBusFunc) on_appsrc_message, this);
    //        gst_object_unref (bus);

    // run the show pipeline
    gst_element_set_state (_app_show_pipeline, GST_STATE_PLAYING);

    _mutex->unlock();

    return true;
}

void PlayerVideo::feed(GstBuffer *buffer)
{
    _mutex->lock();
    if (_app_show_pipeline && !_app_show_buffer) {
        _app_show_buffer = gst_buffer_copy (buffer);
#if 0
        GstFlowReturn ret;
        g_signal_emit_by_name(gplayer->_app_video_source, "push-buffer", gplayer->_app_show_buffer, &ret);
        gst_buffer_unref(gplayer->_app_show_buffer);
        gplayer->_app_show_buffer = NULL;
#endif
    }
    _mutex->unlock();
}

