#include "player_audio.h"

PlayerAudio::PlayerAudio(string name)
{
    _name = name;
}

PlayerAudio::~PlayerAudio()
{

}

/* called when we get a GstMessage from the sink pipeline when we get EOS, we
 * exit the mainloop and this testapp. */
static gboolean on_appsrc_message (GstBus * bus, GstMessage * message, void * data)
{
    /* nil */
    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_EOS:
        g_print ("Finished playback\n");
        break;
    case GST_MESSAGE_ERROR:
        g_print ("Received error\n");
        break;
    default:
        break;
    }
    return TRUE;
}

#if APPSRC_PULL_MODE
static gboolean read_data(ProgramData* app_data)
{
    GstElement *appsink;
    appsink = gst_bin_get_by_name (GST_BIN (app_data->src_pipeline), "testsink");
    GstFlowReturn ret = on_new_sample_from_sink(appsink, app_data);
    gst_object_unref(appsink);

    GstElement *source = NULL;
    if(s_app_buffer){
        g_print("read data()....\n");
        source = gst_bin_get_by_name (GST_BIN (app_data->sink_pipeline), "testsource");
        //ret = gst_app_src_push_buffer (GST_APP_SRC (source), app_buffer);
        g_signal_emit_by_name( source, "push-buffer", s_app_buffer, &ret );//数据送入pipeline
        gst_object_unref (source);
        gst_buffer_unref(s_app_buffer);
        s_app_buffer = NULL;
    }
    else{
        g_print("read_data() s_app_buffer is NULL\n");
    }
    return TRUE;
}

static void on_appsrc_need_data(GstElement *appsrc,
                                guint unused_size,
                                ProgramData* app_data)
{
    g_print("on_appsrc_need_data() call !!!\n");
    guint src_id = g_idle_add ((GSourceFunc) read_data, app_data);
    return;
}
#endif

bool PlayerAudio::start()
{
    GstBus *bus;
    GstStateChangeReturn sret;

    char cmd[256] = {0};

    /* Prepare audio play pipe */
    sprintf(cmd, "appsrc name=testsource ! wavparse ! autoaudiosink ");

    printf("PlayerCore [AUDIO] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _play_audio_pipeline = gst_parse_launch(cmd, NULL);
    if(!_play_audio_pipeline){
        return false;
    }
    _audio_src_buff = gst_bin_get_by_name (GST_BIN (_play_audio_pipeline), "testsource");
    /* configure for time-based format */
    g_object_set (_audio_src_buff, "format", GST_FORMAT_TIME, NULL);
    /* uncomment the next line to block when appsrc has buffered enough */
    /* g_object_set (_audio_src_buff, "block", TRUE, NULL); */
#if APPSRC_PULL_MODE
    g_signal_connect (_audio_src_buff, "need-data", G_CALLBACK (on_appsrc_need_data), data);
#endif
    gst_object_unref (_audio_src_buff);

    bus = gst_element_get_bus (_play_audio_pipeline);
    gst_bus_add_watch (bus, (GstBusFunc) on_appsrc_message, this);
    gst_object_unref (bus);

    // run the audio pipeline
    sret = gst_element_set_state (_play_audio_pipeline,
                                  GST_STATE_PLAYING);

    return true;
}

void PlayerAudio::feed(GstBuffer *buffer)
{
#if !APPSRC_PULL_MODE
    _app_audio_buffer = gst_buffer_copy (buffer);
#else
    s_app_buffer = gst_buffer_copy(buffer);
#endif
    /* 如果appsrc 为push 模式，直接将数据注入给appsrc */
    /* get source an push new buffer */
#if !APPSRC_PULL_MODE
    GstFlowReturn ret;
    GstElement *source = gst_bin_get_by_name (GST_BIN (_play_audio_pipeline), "testsource");
    //ret = gst_app_src_push_buffer (GST_APP_SRC (source), app_buffer);
    g_signal_emit_by_name( source, "push-buffer", _app_audio_buffer, &ret );
    gst_object_unref (source);
    gst_buffer_unref(_app_audio_buffer);
#endif
}
