#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "player_core.h"
#include <QUrl>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    config.load("/opt/config.json");

    int row = 3;
    int col = 3;

    /* 320x240 */
    /* 320x200 */
    /* 240x180 */
    /* 160x120 */
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            players[i * 3 + j] = new PlayerWidget(this, QString().sprintf("Player-%d", i * 3 + j + 1));
            players[i * 3 + j]->setResolution(config.show_width, config.show_height);

            ui->gridLayoutVideos->addWidget(players[i * 3 + j], i, j);
        }
    }

    connect(players[0],SIGNAL(clicked()),this,SLOT(Player0Clicked()));
    connect(players[1],SIGNAL(clicked()),this,SLOT(Player1Clicked()));
    connect(players[2],SIGNAL(clicked()),this,SLOT(Player2Clicked()));
    connect(players[3],SIGNAL(clicked()),this,SLOT(Player3Clicked()));
    connect(players[4],SIGNAL(clicked()),this,SLOT(Player4Clicked()));
    connect(players[5],SIGNAL(clicked()),this,SLOT(Player5Clicked()));
    connect(players[6],SIGNAL(clicked()),this,SLOT(Player6Clicked()));
    connect(players[7],SIGNAL(clicked()),this,SLOT(Player7Clicked()));
    connect(players[8],SIGNAL(clicked()),this,SLOT(Player8Clicked()));

    //首页timer初始化
    screensaver_timer = new QTimer();
    connect(screensaver_timer,SIGNAL(timeout()),this,SLOT(ScreenSaverTimeout()));
    screensaver_timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type()!=QEvent::WindowStateChange)
        return;

    if(this->windowState()==Qt::WindowMaximized)
    {

    }
}

int firstStart = 0;
bool flag = false;

void MainWindow::ScreenSaverTimeout()
{
    PlayerCore::MediaMetaData media;
    screensaver_timer->stop();

    // 创建视频墙
    if (firstStart == 0) {
        // 解析运行参数
        QStringList arguments = QCoreApplication::arguments();

        for (int i = 0; i < arguments.size(); i++) {
            printf("Arguments : %d %s\n", i, arguments[i].toStdString().c_str());

            media.file_path = arguments[i].toStdString().c_str();

            if (config.media.type == PlayerCore::RTP) {
                media.type = PlayerCore::RTP;
                media.port = 5000;
                media.sdp = "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264";
            }
            else if (config.media.type == PlayerCore::RTSP) {
                media.type = PlayerCore::RTSP;
            }
            else if (config.media.type == PlayerCore::MP4) {
                media.type = PlayerCore::MP4;
            }
            else {
                media.type = PlayerCore::I420;
                media.resolution.setWidth(config.media.resolution.width());
                media.resolution.setHeight(config.media.resolution.height());
            }

            if (i == 0)
                continue;
            else {
                players[i - 1]->play(media, PlayerCore::Mute, config.with_record ? PlayerCore::RecordOn : PlayerCore::RecordOff);
            }
        }

        firstStart++;
    }
}

void MainWindow::showAllVideo() {
    for (int i = 0; i < 9; i++) {
        if (players[i]->isVisible()) {
            ui->gridLayoutVideos->removeWidget(players[i]);
            players[i]->hide();
            players[i]->setResolution(config.show_width, config.show_height);
            //             players[i]->replay();
            //             players[i]->stop();
        }
    }

    int row = 3;
    int col = 3;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            ui->gridLayoutVideos->addWidget(players[i * 3 + j], i, j);
            players[i * 3 + j]->show();
        }
    }

    ui->gridLayoutVideos->update();
}

void MainWindow::showOneVideo(int number)
{
    for (int i = 0; i < 9; i++) {
        ui->gridLayoutVideos->removeWidget(players[i]);
        players[i]->hide();
        //        players[i]->stop();
    }

    ui->gridLayoutVideos->addWidget(players[number], 0, 0);
    players[number]->setResolution(config.show_large_width, config.show_large_height);
    //    players[number]->replay();
    players[number]->show();

    ui->gridLayoutVideos->update();
}

void MainWindow::Player0Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(0);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}

void MainWindow::Player1Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(1);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}

void MainWindow::Player2Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(2);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}
void MainWindow::Player3Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(3);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}
void MainWindow::Player4Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(4);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}
void MainWindow::Player5Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(5);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}
void MainWindow::Player6Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(6);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}
void MainWindow::Player7Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(7);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}
void MainWindow::Player8Clicked()
{
    static bool flag = false;

    if (!flag) {
        showOneVideo(8);
        flag = !flag;
    }
    else {
        showAllVideo();
        flag = !flag;
    }
}


