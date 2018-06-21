#include "player_record.h"

PlayerRecord::PlayerRecord(string name)
{
    _name = name;
    _record_mutex = new QMutex();
    _app_record_pipeline = NULL;
}

gboolean PlayerRecord::_APPRecordFeedDataCall(void* app_data)
{
    //    printf("_APPRecordFeedDataCall\n");
    PlayerRecord * gplayer = (PlayerRecord*)app_data;

    gplayer->_record_mutex->lock();
    if (gplayer->_app_record_pipeline && gplayer->_app_record_buffer) {
        GstFlowReturn ret;
        g_signal_emit_by_name(gplayer->_app_record_source, "push-buffer", gplayer->_app_record_buffer, &ret);
        if (ret != GST_FLOW_OK) {
            /* some error, stop sending data */
            printf("### PlayerRecord %s record fail feed data!\n", gplayer->_name.c_str());
        }
        gst_buffer_unref(gplayer->_app_record_buffer);
        gplayer->_app_record_buffer = NULL;
    }
    gplayer->_record_mutex->unlock();

    usleep(20);

    return TRUE;
}

void PlayerRecord::_APPRecordOnNeedDataCall(GstElement *appsrc,
                                         guint unused_size,
                                         void* app_data)
{
    PlayerRecord * gplayer = (PlayerRecord*)app_data;
    //    printf("PlayerRecord %s record need data!\n", gplayer->_name.c_str());
    if (gplayer->_app_record_src_id == 0) {
        gplayer->_app_record_src_id = g_idle_add ((GSourceFunc) _APPRecordFeedDataCall, app_data);
    }

    usleep(10);

    return;
}

void PlayerRecord::_APPRecordOnEnoughDataCall(GstElement * pipeline, void *app_data)
{
    PlayerRecord * gplayer = (PlayerRecord*)app_data;
    //    printf("###PlayerRecord %s record enough data!\n", gplayer->_name.c_str());
    if (gplayer->_app_record_src_id != 0) {
        g_source_remove (gplayer->_app_record_src_id);
        gplayer->_app_record_src_id = 0;
    }

    usleep(10);

    return;
}

bool PlayerRecord::start(int mediaWidth, int mediaHeight)
{
    _mediaWidth = mediaWidth;
    _mediaHeight = mediaHeight;

    if (_mediaWidth == -1 || _mediaHeight == -1)
        return false;

    if (_app_record_pipeline) {
        gst_element_set_state (_app_record_pipeline, GST_STATE_NULL);
        gst_object_unref (_app_record_pipeline);
        _app_record_pipeline = NULL;
    }

    char cmd[256] = {0};
    GstBus *bus = NULL;

    /* Prepare audio play pipe */
    sprintf(cmd, "appsrc name=recordsource ! videoparse width=%d height=%d ! queue ! videorate max-rate=20 ! vpuenc_h264 ! matroskamux ! filesink location=/opt/mv/record/%s.mkv ",
            _mediaWidth, _mediaHeight, _name.c_str());

    printf("PlayerRecord [RECORD] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _app_record_pipeline = gst_parse_launch(cmd, NULL);
    if(!_app_record_pipeline){
        return false;
    }

    _app_record_source = gst_bin_get_by_name (GST_BIN(_app_record_pipeline), "recordsource");
    if(!_app_record_source){
        printf("[ERROR] GPlayer gst_bin_get_by_name fail!\n");
        return false;
    }

    g_object_set (_app_record_source, "format", GST_FORMAT_TIME, NULL);

#ifndef VIDEO_PULL
    g_signal_connect(_app_record_source, "need-data", G_CALLBACK (_APPRecordOnNeedDataCall), this);
    g_signal_connect(_app_record_source, "enough-data", G_CALLBACK(_APPRecordOnEnoughDataCall), this);
#endif

    // run the show pipeline
    gst_element_set_state (_app_record_pipeline, GST_STATE_PLAYING);

    return true;
}

void PlayerRecord::feed(GstBuffer *buffer)
{
    _record_mutex->lock();
    if (_app_record_pipeline && !_app_record_buffer) {
        _app_record_buffer = gst_buffer_copy (buffer);
    }
    _record_mutex->unlock();
}
