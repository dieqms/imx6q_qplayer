#include "player_widget.h"

PlayerWidget::PlayerWidget(QWidget *parent, QString name)
    : GLWidget(parent, name),
      _name(name)
{
    _hiden = false;
    _resolution = new QSize(800, 480);
    _core = new PlayerCore(_name.toStdString());
    _core->Init(0, NULL, _videoProcessor, this, *_resolution);
}

bool PlayerWidget::setResolution(int width, int height)
{
    if (_resolution)
        delete _resolution;

    _resolution = new QSize(width, height);
    _core->SetResolution(*_resolution);

    return true;
}

PlayerWidget::~PlayerWidget()
{
    if (_core)
        delete _core;

    if (_resolution)
        delete _resolution;
}

void PlayerWidget::_videoProcessor(unsigned char* data,int len, void * cookie)
{
    PlayerWidget * player = (PlayerWidget*)cookie;

    if (player->_hiden)
        return;

    // 显示
    int width = player->_resolution->width();
    int height =  player->_resolution->height();
    player->updateWidget(data, len, width, height, QImage::Format_RGB16);

    // 保存文件

    //    printf("UI Got data %d\n", len);
    //    unsigned char blank[320 * 240 / 2] = {0};
    //    ui->paintLabel->update(data, len, width, height, QImage::Format_RGB16);

    //    Base::FileUtil::WriteAllBytes("/opt/test.yuv", data, width * height, true);
    //    Base::FileUtil::WriteAllBytes("/opt/test.yuv", blank, width * height / 2, true);

    //Base::FileUtil::WriteAllBytes(QString().asprintf("/opt/mnt/%s.yuv", player->_name.toStdString().c_str()).toStdString().c_str(), data, len, true);

    return;
}

bool PlayerWidget::play(PlayerCore::MediaMetaData media, PlayerCore::AudioMute mute, PlayerCore::RecordOnOrOff record)
{
    _currMedia = media;

    //Base::FileUtil::Delete(QString().asprintf("/opt/mnt/%s.yuv", _name.toStdString().c_str()).toStdString().c_str());

    _core->Stop();
    _core->Play(media, mute, record);

//     setUpdatesEnabled(true);

     return true;
}

bool PlayerWidget::replay()
{
    return play(_currMedia);
}

void PlayerWidget::hide()
{
    _hiden = true;
    setVisible(false);
}

void PlayerWidget::show()
{
    _hiden = false;
    setVisible(true);
}

void PlayerWidget::update()
{
    GLWidget::update();
}

void PlayerWidget::stop()
{
    _core->Stop();
}


