#include "player_core.h"

#define APPSRC_PULL_MODE (0)  /* appsrc working in pull mode or not */

Cache::Cache(AVProcessor processor, void *cookie) : Pthread(NULL, NULL, "Cache")
{
    _processor = processor;
    _cookie = cookie;
}

void Cache::push(string data) {
    _mutex.lock();
    _cache.push_back(data);
    _mutex.unlock();
}

void Cache::push(char * data, size_t len) {
    string str((const char*)data, len);
    _mutex.lock();
    _cache.push_back(str);
    _mutex.unlock();
}

void Cache::runner(void * var) {
    while (is_run()) {
        string data;
        bool have_data = false;

        _mutex.lock();
        if (_cache.empty()) {
            data = "";
            have_data = false;
        }
        else {
            have_data = true;
            data = _cache.front();
            _cache.pop_front();
        }
        _mutex.unlock();

        if (!have_data) {
            usleep(100);
            continue;
        }

        //        printf("Cache save %d data\n", data.size());

        if (_processor)
            _processor((unsigned char*)data.data(), data.size(), _cookie);
        //        emit newData((unsigned char*)data.data(), data.size());

        usleep(25 * 1000);
    }
}

bool PlayerCore::_gst_inited = false;
GMainLoop * PlayerCore::_loop = NULL;
Pthread * PlayerCore::_gst_loop_thread = NULL;

void PlayerCore::_GstLooper(void *) {
    g_main_loop_run(PlayerCore::_loop);
}

PlayerCore::PlayerCore(string name) : Pthread(NULL, NULL, "GPlayer")
{
    _name = name;
    _video_cacher = NULL;
    _video_pipeline = NULL;
    _audio_pipeline = NULL;
    _video_sink_buff = NULL;
    _audio_sink_buff = NULL;
    _app_audio_buffer = NULL;

    if (!PlayerCore::_gst_inited) {
        PlayerCore::_gst_inited = true;
        gst_init(0, NULL);

        if (!g_thread_supported ())
            g_thread_init (NULL);

        PlayerCore::_loop = g_main_loop_new(NULL, FALSE);
        PlayerCore::_gst_loop_thread = new Pthread(_GstLooper, NULL, "Gloop");
        _gst_loop_thread->start();
    }

    _mutex = new QMutex();
    _video = new PlayerVideo(name);
    _audio = new PlayerAudio(name);
    _recorder = new PlayerRecord(name);
}

PlayerCore::~PlayerCore()
{
//    if (_loop) {
//        g_main_loop_quit(_loop);
//        g_main_loop_unref(_loop);
//        _loop = NULL;

//        stop();
//    }

    Stop();

    if (_video_cacher)
        delete _video_cacher;
    if (_mutex)
        delete _mutex;
    if (_video)
        delete _video;
    if (_audio)
        delete _audio;
    if (_recorder)
        delete _recorder;
}

bool PlayerCore::Init(int argc, char *argv[], AVProcessor video_processor, void * video_processor_cookie, QSize video_resolution)
{
    _video_resolution = video_resolution;
    _video->init(video_processor, video_processor_cookie, video_resolution);
    return true;
}

void PlayerCore::SetResolution(QSize resolution)
{
    _video_resolution = resolution;
    _video->setResolution(resolution);
}

/* called when the appsink notifies us that there is a new buffer ready for
* processing */
void PlayerCore::_APPVideoSinkCall(GstElement * elt, void *ptr)
{
    //printf("_APPRawDataSinkCall \n ");

    PlayerCore * gplayer = (PlayerCore*)ptr;

    //    guint size;
    GstBuffer *buffer;
    GstMapInfo map;
    GstSample *sample;

    /* get the buffer from appsink */
    g_signal_emit_by_name(gplayer->_video_sink_buff, "pull-sample", &sample, NULL);
    if (sample)
    {
#if 0
        GstCaps *caps;
        GstStructure *s;
        gint width, height;
        gboolean res;

        /* get the snapshot buffer format now. We set the caps on the appsink so
         * that it can only be an rgb buffer. The only thing we have not specified
         * on the caps is the height, which is dependant on the pixel-aspect-ratio
         * of the source material */
        caps = gst_sample_get_caps (sample);
        if (!caps) {
            printf("could not get snapshot format\n");
        }
        s = gst_caps_get_structure (caps, 0);

        /* we need to get the final caps on the buffer to get the size */
        res = gst_structure_get_int (s, "width", &width);
        res |= gst_structure_get_int (s, "height", &height);
        if (!res) {
            printf("could not get snapshot dimension\n");
        }
#endif

        memset(&map, 0, sizeof(GstMapInfo));
        buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(buffer, &map, GST_MAP_READ);


#if 0
        //        printf("\n sample got of size = %d, width = %d, height = %d\n", map.size, width, height);
        //        gplayer->_video_cacher->push((char *)map.data, map.size);
        gplayer->_video_processor((unsigned char *)map.data, map.size, gplayer->_video_processor_cookie);
#else
        // 显示
        gplayer->_video->feed(buffer);

        // 录制
        gplayer->_recorder->feed(buffer);
#endif

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
    }
}

/* called when the appsink notifies us that there is a new buffer ready for
* processing */
void PlayerCore::_APPAudioSinkCall(GstElement * elt, void *ptr)
{
    //printf("GSTOnNewSampleFromSink \n ");

    PlayerCore * gplayer = (PlayerCore*)ptr;

    //    guint size;
    GstBuffer *buffer;
    GstMapInfo map;
    GstSample *sample;

    /* get the buffer from appsink */
    g_signal_emit_by_name(gplayer->_audio_sink_buff, "pull-sample", &sample, NULL);
    if (sample)
    {
        memset(&map, 0, sizeof(GstMapInfo));
        buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        gplayer->_audio->feed(buffer);

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
    }
}

gboolean PlayerCore::_APPRawPipeBusCall(GstBus *bus, GstMessage *msg, gpointer data)
{
    PlayerCore * gplayer = (PlayerCore*)data;
    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        printf("GSTBusCall %s: End of stream\n", gplayer->_name.c_str());
        gplayer->Seek();
//                gplayer->Stop();
        break;

    case GST_MESSAGE_ERROR:
    {
        gchar  *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);

        printf("GSTBusCall %s Error: %s\n", gplayer->_name.c_str(), error->message);

        g_error_free(error);
        g_free(debug);

        gplayer->Stop();
        break;
    }

    default:
        break;
    }
    return TRUE;
}

bool PlayerCore::Play(MediaMetaData media, AudioMute mute, RecordOnOrOff record)
{
    if (media.type == I420) {
        _video->start(media.resolution.width(), media.resolution.height());
        if (record == RecordOn)
            _recorder->start(media.resolution.width(), media.resolution.height());
        return _DecodeYUV(media.file_path.c_str(), media.resolution.width(), media.resolution.height());
    }
    else if (media.type == MP4) {
        _video->start(1280, 720, true);
        return _DecodeMp4(media.file_path.c_str(), mute);
    }
    else if (media.type == RTP) {
        _video->start(1280, 720, true);
        return _DecodeRTP(media.port, media.sdp.c_str());
    }
    else if (media.type == RTSP) {
        _video->start(1280, 720, true);
        return _DecodeRTSP(media.file_path.c_str());
    }

    return false;
}

bool PlayerCore::_DecodeMp4(const char* file, AudioMute mute)
{
    GstBus *bus;
    GstStateChangeReturn sret;

    char cmd[256] = {0};

    if (mute == UnMute) {
        sprintf(cmd, "filesrc location=/%s ! qtdemux name=demux "
                     " demux.audio_0 ! queue ! decodebin ! wavenc ! appsink name = testsink "
                     " demux.video_0 ! queue ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=%d, height=%d ! appsink name = sink sync=true ",
                file, 1280, 720);
    }
    else {
        sprintf(cmd, "filesrc location=/%s ! qtdemux name=demux "
                     " demux.video_0 ! queue ! vpudec ! imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=%d, height=%d ! appsink name = sink sync=true",
                file, 1280, 720);
    }

    printf("PlayerCore [DECODER] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _video_pipeline = gst_parse_launch(cmd, NULL);
    if(!_video_pipeline){
        return false;
    }

    /* get sink */
    _video_sink_buff = gst_bin_get_by_name(GST_BIN(_video_pipeline), "sink");
    g_object_set(G_OBJECT(_video_sink_buff), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect(_video_sink_buff, "new-sample", G_CALLBACK(_APPVideoSinkCall), this);

    /* Set up the pipeline */
    /* we add a message handler */
    bus = gst_pipeline_get_bus(GST_PIPELINE(_video_pipeline));
    gst_bus_add_watch(bus, _APPRawPipeBusCall, this);
    gst_object_unref(bus);

    if (mute == UnMute) {
        /* audio sink */
        _audio_sink_buff = gst_bin_get_by_name(GST_BIN(_video_pipeline), "testsink");
        g_object_set(G_OBJECT(_audio_sink_buff), "emit-signals", TRUE, "sync", FALSE, NULL);
        g_signal_connect(_audio_sink_buff, "new-sample", G_CALLBACK(_APPAudioSinkCall), this);

        _audio->start();
    }

    // run the pipeline
    sret = gst_element_set_state (_video_pipeline, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state (_video_pipeline, GST_STATE_NULL);
        gst_object_unref (_video_pipeline);
        printf("Gstreamer play fail!\n");
    }

    return true;
}

bool PlayerCore::_DecodeYUV(const char* file, int mediaWidth, int mediaHeight)
{
    char cmd[256] = {0};

    GstBus *bus = NULL;

//    sprintf(cmd, "filesrc location=/%s ! videoparse width=%d height=%d framerate=20 ! imxvideoconvert_g2d ! video/x-raw, format=RGB16 ! appsink name = %s sync=false",
//            file, mediaWidth, mediaHeight, _name.c_str());
    sprintf(cmd, "filesrc location=/%s ! videoparse width=%d height=%d ! videorate max-rate=20 ! appsink name = %s sync=false",
            file, mediaWidth, mediaHeight, _name.c_str());

    printf("PlayerCore [DECODER] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _video_pipeline = gst_parse_launch(cmd, NULL);
    if(!_video_pipeline){
        printf("[ERROR] GPlayer gst_parse_launch fail!\n");
        return false;
    }

    /* get sink */
    _video_sink_buff = gst_bin_get_by_name(GST_BIN(_video_pipeline), _name.c_str());
    if(!_video_pipeline){
        printf("[ERROR] GPlayer gst_bin_get_by_name fail!\n");
        return false;
    }

    g_object_set(G_OBJECT(_video_sink_buff), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect(_video_sink_buff, "new-sample", G_CALLBACK(_APPVideoSinkCall), this);

    /* Set up the pipeline */
    /* we add a message handler */
    bus = gst_pipeline_get_bus(GST_PIPELINE(_video_pipeline));
    if(!bus){
        printf("[ERROR] GPlayer gst_pipeline_get_bus fail!\n");
        return false;
    }

    gst_bus_add_watch(bus, _APPRawPipeBusCall, this);
    gst_object_unref(bus);

    // run the pipeline
    GstStateChangeReturn sret = gst_element_set_state (_video_pipeline, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state (_video_pipeline, GST_STATE_NULL);
        gst_object_unref (_video_pipeline);
        printf("[ERROR] GPlayer play fail!\n");
        return false;
    }

    return true;
}

bool PlayerCore::_DecodeRTP(int port, const char* sdp)
{
    char cmd[256] = {0};

    GstBus *bus = NULL;

    sprintf(cmd, "udpsrc port=%d caps=\"%s\" ! rtph264depay ! vpudec !"
                 " imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=%d, height=%d ! appsink name = %s sync=true ",
                    port, sdp, 1280, 720, _name.c_str());
    printf("PlayerCore [DECODER] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _video_pipeline = gst_parse_launch(cmd, NULL);
    if(!_video_pipeline){
        printf("[ERROR] GPlayer gst_parse_launch fail!\n");
        return false;
    }

    /* get sink */
    _video_sink_buff = gst_bin_get_by_name(GST_BIN(_video_pipeline), _name.c_str());
    if(!_video_pipeline){
        printf("[ERROR] GPlayer gst_bin_get_by_name fail!\n");
        return false;
    }

    g_object_set(G_OBJECT(_video_sink_buff), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect(_video_sink_buff, "new-sample", G_CALLBACK(_APPVideoSinkCall), this);

    /* Set up the pipeline */
    /* we add a message handler */
    bus = gst_pipeline_get_bus(GST_PIPELINE(_video_pipeline));
    if(!bus){
        printf("[ERROR] GPlayer gst_pipeline_get_bus fail!\n");
        return false;
    }

    gst_bus_add_watch(bus, _APPRawPipeBusCall, this);
    gst_object_unref(bus);

    // run the pipeline
    GstStateChangeReturn sret = gst_element_set_state (_video_pipeline, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state (_video_pipeline, GST_STATE_NULL);
        gst_object_unref (_video_pipeline);
        printf("[ERROR] GPlayer play fail!\n");
        return false;
    }

    return true;
}

bool PlayerCore::_DecodeRTSP(const char* uri)
{
    char cmd[256] = {0};

    GstBus *bus = NULL;

    sprintf(cmd, "rtspsrc location=\"%s\" ! rtph264depay ! vpudec ! "
                 " imxvideoconvert_g2d ! video/x-raw, format=RGB16, width=%d, height=%d ! appsink name = %s sync=true ",
            uri, 1280, 720, _name.c_str());
    printf("PlayerCore [DECODER] %s cmd: gst-launch-1.0 %s\n", _name.c_str(), cmd);

    /* Build the pipeline */
    _video_pipeline = gst_parse_launch(cmd, NULL);
    if(!_video_pipeline){
        printf("[ERROR] GPlayer gst_parse_launch fail!\n");
        return false;
    }

    /* get sink */
    _video_sink_buff = gst_bin_get_by_name(GST_BIN(_video_pipeline), _name.c_str());
    if(!_video_pipeline){
        printf("[ERROR] GPlayer gst_bin_get_by_name fail!\n");
        return false;
    }

    g_object_set(G_OBJECT(_video_sink_buff), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect(_video_sink_buff, "new-sample", G_CALLBACK(_APPVideoSinkCall), this);

    /* Set up the pipeline */
    /* we add a message handler */
    bus = gst_pipeline_get_bus(GST_PIPELINE(_video_pipeline));
    if(!bus){
        printf("[ERROR] GPlayer gst_pipeline_get_bus fail!\n");
        return false;
    }

    gst_bus_add_watch(bus, _APPRawPipeBusCall, this);
    gst_object_unref(bus);

    // run the pipeline
    GstStateChangeReturn sret = gst_element_set_state (_video_pipeline, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state (_video_pipeline, GST_STATE_NULL);
        gst_object_unref (_video_pipeline);
        printf("[ERROR] GPlayer play fail!\n");
        return false;
    }

    return true;
}

void PlayerCore::Stop()
{
    _mutex->lock();

    if (_video_pipeline) {
        printf("PlayerCore %s stop!\n", _name.c_str());
        gst_element_set_state (_video_pipeline, GST_STATE_NULL);
        gst_object_unref (_video_pipeline);
        _video_pipeline = NULL;
    }

    _mutex->unlock();
}

void PlayerCore::runner(void * var) {
    g_main_loop_run(_loop);
}

void PlayerCore::Seek(int64_t pos) {
    _mutex->lock();
    if (_video_pipeline) {
        //    gst_element_seek(_pipeline, 1, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
        //                      GST_SEEK_TYPE_SET, 0 * GST_MSECOND * 1000,
        //                      GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE);

        gst_element_seek_simple (_video_pipeline, GST_FORMAT_TIME,
                                 (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), pos * GST_SECOND);
    }
    _mutex->unlock();
}




