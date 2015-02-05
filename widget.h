#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "about.h"
#include <QStringListModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDir>
#include <QFileDialog>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QSystemTrayIcon>

//#define private public //if unit test

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_btnAbout_clicked();
    void vievCurrentTrack(QModelIndex);
    void onShowVolumeToolTip(int);
    void setPositionSlider(qint64);
    void initDuration(qint64);
    void changePositionSlider();
    void disableConnectPositionSlider();
    void errorMessage(QMediaPlayer::Error);
    void playerStatusChanged(QMediaPlayer::MediaStatus);
    void volumeUp();
    void volumeDown();
    void setPlayMediaUpdateTrackName(QVariant&, int);

    void clickTray(QSystemTrayIcon::ActivationReason);
    void trayMessageClick();

    void on_btnPause_clicked();

    void on_btnPlay_clicked();

    void on_btnStop_clicked();

    void on_btnPrev_clicked();

    void on_btnNext_clicked();

    void on_btnOpenDialogDir_clicked();

    void on_trackListView_doubleClicked(const QModelIndex &index);

signals:
    void audioPlay();
    void audioPause();
    void audioStop();


private:
    Ui::Widget *ui;
    About* about = NULL; // Форма о программе

    //player
    QMediaPlayer* audioPlayer;
    QStringListModel* trackListModel;
    QFileDialog* dirDialog;
    QDir* dir;        // Текущий каталог с композициями
    QStringList* stringList;
    QStringList* nameFilters;
    int currentTrack; // Номер текущей композиции в списке
    int volumeStep;   // Шаг изменения громкости горячей кнопки
    //Icon
    QIcon* iconMain;
    QIcon* iconPlay;
    QIcon* iconPause;
    QIcon* iconStop;

    QSystemTrayIcon* trayIcon;
    //Short key
    QShortcut *hotKeyPlay;
    QShortcut *hotKeyPause;
    QShortcut *hotKeyStop;
    QShortcut *hotKeyPrev;
    QShortcut *hotKeyNext;
    QShortcut *hotKeyVolUp;
    QShortcut *hotKeyVolDown;
};

#endif // WIDGET_H
