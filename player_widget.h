#ifndef PLAYER_H
#define PLAYER_H

#include "player_core.h"
#include "glwidget.h"

class PlayerWidget : public GLWidget
{
public:
    explicit PlayerWidget(QWidget *parent = 0, QString name = "");
    ~PlayerWidget();

    // set video quality
    bool setResolution(int width = 720, int height = 480);

    // play media
    bool play(PlayerCore::MediaMetaData media, PlayerCore::AudioMute mute = PlayerCore::UnMute, PlayerCore::RecordOnOrOff record = PlayerCore::RecordOff);

    bool replay();

    void stop();

    void hide();
    void show();
    void update();

private:
    static void _videoProcessor(unsigned char *data, int _imageDataLen, void *cookie);

    // gstreamer
    PlayerCore *    _core;
    // video quality
    QSize   *       _resolution;
    // player name
    QString         _name;

    PlayerCore::MediaMetaData   _currMedia;

    bool            _hiden;

    friend void _videoProcessor(unsigned char *data, int _imageDataLen, void *cookie);
};


#endif // PLAYER_H
