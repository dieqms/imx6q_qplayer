#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "glyuvwidget.h"
#include "glwidget.h"
#include "paintlabel.h"
#include "player_widget.h"
#include "player_core.h"
#include "config.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent * event);
    void showAllVideo();
    void hideAllVideo();
    void showOneVideo(int number);
    void hideOneVideo(int number);

private slots:
    void ScreenSaverTimeout();

    void Player0Clicked();
    void Player1Clicked();
    void Player2Clicked();
    void Player3Clicked();
    void Player4Clicked();
    void Player5Clicked();
    void Player6Clicked();
    void Player7Clicked();
    void Player8Clicked();

private:
    Ui::MainWindow *ui;
    QTimer * screensaver_timer;
    PlayerWidget * players[9];
    Config config;
};

#endif // MAINWINDOW_H
