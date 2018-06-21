#ifndef CONFIG_H
#define CONFIG_H
#include "base/json_util.hpp"
#include "player_core.h"

class Config
{
public:
    Config()
    {
        show_width = 320;
        show_height = 240;
        show_large_width = 720;
        show_large_height = 480;
        with_record = false;
    }

    bool load(const char * path) {
        Base::JsonUtil json;
        if (!json.ParseFile(path)) {
            printf("Config load %s fail!\n", path);
            return false;
        }

        string type;
        if (!json.GetString("meida_type", type)) {
            printf("Config load meida_type fail!\n");
            return false;
        }

        int meida_width;
        if (!json.GetNumber("meida_width", meida_width)) {
            printf("Config load meida_width fail!\n");
            return false;
        }

        int meida_height;
        if (!json.GetNumber("meida_height", meida_height)) {
            printf("Config load meida_height fail!\n");
            return false;
        }

        string sdp;
        if (!json.GetString("meida_sdp", sdp)) {
            printf("Config load meida_sdp fail!\n");
            return false;
        }

        if (!json.GetBool("with_record", with_record)) {
            printf("Config load with_record fail!\n");
            return false;
        }

        if (!json.GetNumber("show_width", show_width)) {
            printf("Config load show_width fail!\n");
            return false;
        }

        if (!json.GetNumber("show_height", show_height)) {
            printf("Config load show_height fail!\n");
            return false;
        }

        if (!json.GetNumber("show_large_width", show_large_width)) {
            printf("Config load show_large_width fail!\n");
            return false;
        }

        if (!json.GetNumber("show_large_height", show_large_height)) {
            printf("Config load show_large_height fail!\n");
            return false;
        }

        if (type == "I420") {
            media.type = PlayerCore::I420;
            printf("Config: media type I420\n");
        }
        else if (type == "MP4") {
            media.type = PlayerCore::MP4;
            printf("Config: media type MP4\n");
        }

        media.resolution.setWidth(meida_width);
        media.resolution.setHeight(meida_height);
        printf("Config: media width %d height %d\n", meida_width, meida_height);

        media.sdp = sdp;
        printf("Config: media sdp %s\n", sdp.c_str());

        printf("Config: show width %d height %d, large width %d height %d.\n", show_width, show_height, show_large_width, show_large_height);

        printf("Config: record %s\n", with_record ? "true" : "false");
        return true;
    }

    PlayerCore::MediaMetaData media;

    bool with_record;
    int show_width;
    int show_height;
    int show_large_width;
    int show_large_height;
};

#endif // CONFIG_H
