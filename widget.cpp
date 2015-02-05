#include "widget.h"
#include "ui_widget.h"
#include <QToolTip>
#include <QAudioBuffer>
#include <QMessageBox>
#include <QMediaMetaData>
#include <QSystemTrayIcon>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    currentTrack(0),
    volumeStep(5)
{
    ui->setupUi(this);

    trackListModel = new QStringListModel();

    iconPlay = new QIcon(QString::fromUtf8(":/icon/images/continue_256.png"));
    iconPause = new QIcon(QString::fromUtf8(":/icon/images/pause_256.png"));
    iconStop = new QIcon(QString::fromUtf8(":/icon/images/stop_256.png"));
    iconMain = new QIcon(QString::fromUtf8(":/icon/images/Headphones.png"));

    // HotKey
    hotKeyPlay = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Z), this);
    hotKeyPause = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_X), this);
    hotKeyStop = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_C), this);
    hotKeyPrev = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_V), this);
    hotKeyNext = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_B), this);
    hotKeyVolUp = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Q), this);
    hotKeyVolDown = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_A), this);
    // Connect hotkey
    connect(hotKeyPlay, SIGNAL(activated()), ui->btnPlay, SLOT(click()));
    connect(hotKeyPause, SIGNAL(activated()), ui->btnPause, SLOT(click()));
    connect(hotKeyStop, SIGNAL(activated()), ui->btnStop, SLOT(click()));
    connect(hotKeyPrev, SIGNAL(activated()), ui->btnPrev, SLOT(click()));
    connect(hotKeyNext, SIGNAL(activated()), ui->btnNext, SLOT(click()));
    connect(hotKeyVolUp, SIGNAL(activated()), this, SLOT(volumeUp()));
    connect(hotKeyVolDown, SIGNAL(activated()), this, SLOT(volumeDown()));

    trayIcon = new QSystemTrayIcon(*iconMain);
    trayIcon->setToolTip("APlayer v1.0");
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(clickTray(QSystemTrayIcon::ActivationReason)));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(trayMessageClick()));
    //Init dir
    dir = new QDir();
    //Init dialog
    dirDialog = new QFileDialog(this, "Select dir");
    nameFilters = new QStringList("*.mp3");


    audioPlayer = new QMediaPlayer();

    audioPlayer->setVolume(ui->VolumeSlider->value());

    connect(this, SIGNAL(audioPlay()), audioPlayer, SLOT(play()));
    connect(this, SIGNAL(audioPause()), audioPlayer, SLOT(pause()));
    connect(this, SIGNAL(audioStop()), audioPlayer, SLOT(stop()));

    connect(ui->VolumeSlider, SIGNAL(valueChanged(int)), audioPlayer, SLOT(setVolume(int)));
    connect(audioPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(setPositionSlider(qint64)));

    stringList = new QStringList();

    ui->trackListView->setModel(trackListModel);

    connect(ui->VolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onShowVolumeToolTip(int)));
    //connect(ui->trackListView, SIGNAL(pressed(QModelIndex)), this, SLOT(vievCurrentTrack(QModelIndex)));
    connect(ui->PositionSlider, SIGNAL(sliderReleased()), this, SLOT(changePositionSlider()));
    connect(ui->PositionSlider, SIGNAL(sliderPressed()), this, SLOT(disableConnectPositionSlider()));

    connect(audioPlayer, SIGNAL(durationChanged(qint64)),this,SLOT(initDuration(qint64)));

    connect(audioPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(errorMessage(QMediaPlayer::Error)));
    connect(audioPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(playerStatusChanged(QMediaPlayer::MediaStatus)));

}

Widget::~Widget()
{
    delete ui;
    if(about != NULL)
        delete about;

    delete trackListModel;

    delete audioPlayer;

    delete dirDialog;
    delete dir;

    delete stringList;
    delete nameFilters;


    delete iconMain;
    delete iconPlay;
    delete iconPause;
    delete iconStop;

    delete trayIcon;

    delete hotKeyPlay;
    delete hotKeyPause;
    delete hotKeyStop;
    delete hotKeyPrev;
    delete hotKeyNext;
    delete hotKeyVolUp;
    delete hotKeyVolDown;

}

// Отображение окна о программе
void Widget::on_btnAbout_clicked()
{
    if(about == NULL)
    {
        about = new About((QWidget*)this->parent());
        about->setWindowModality(Qt::ApplicationModal);
    }
    about->show();
}

// Одно нажатие на список композиций [ОТКЛЮЧЕНО]
void Widget::vievCurrentTrack(QModelIndex index)
{
     QVariant var = trackListModel->data(index, Qt::DisplayRole);
     if(var.isValid())
         ui->labelTrackName->setText(var.toString().length() < 27 ? var.toString() : var.toString().left(27)+"...");
}

// Отображение уровня громкости во время прокрутки
void Widget::onShowVolumeToolTip(int i)
{

    QPoint p(this->pos().rx() + ui->VolumeSlider->pos().rx() - 20,
             this->pos().ry() + ui->VolumeSlider->pos().ry() - 10);

    QToolTip::showText(p, QString("Volume: %1 %").arg(i), this);
}

// Установить размер слайдера в секундах
void Widget::initDuration(qint64 i)
{
    //qDebug("DURATION change %d", i);
    ui->PositionSlider->setMaximum(i/1000);
}

// Отображение текущей позиции слайдера
void Widget::setPositionSlider(qint64 i)
{
    //qDebug("duration %d", i);
    ui->PositionSlider->setValue(i / 1000);

}

// Отключение сигнала от слота во время касания его курсором
void Widget::disableConnectPositionSlider()
{
    //Для плавного передвижения нужно чтобы не показывал текущую позицию слайдер
    disconnect(audioPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(setPositionSlider(qint64)));
}

// Изменения позиции (прокрутка песни)
void Widget::changePositionSlider()
{
    qint64 qi = ui->PositionSlider->value() * 1000;
    audioPlayer->setPosition(qi);
    connect(audioPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(setPositionSlider(qint64)));
}

// Нажатие на кнопку Play
void Widget::on_btnPlay_clicked()
{
    if(audioPlayer->media().isNull())
    {
        QMessageBox::information(this, "Error", "You didn't select dir", QMessageBox::Ok);
        return;
    }

    this->setWindowIcon(*iconPlay);
    emit audioPlay();
}

// Нажатие на кнопку Pause
void Widget::on_btnPause_clicked()
{
    this->setWindowIcon(*iconPause);
    emit audioPause();
}

// Нажатие на кнопку Stop
void Widget::on_btnStop_clicked()
{
    this->setWindowIcon(*iconStop);
    emit audioStop();
}

// Нажатие на кнопку Prev
void Widget::on_btnPrev_clicked()
{
    audioPlayer->stop();
    if(currentTrack <= 0) currentTrack = trackListModel->rowCount();
    const QModelIndex& i = trackListModel->index(--currentTrack);
    ui->trackListView->setCurrentIndex(i);
    QVariant var = trackListModel->data(i, Qt::DisplayRole);
    if(var.isValid())
    {
        setPlayMediaUpdateTrackName(var, 27);
    }
    else
        QMessageBox::information(this, "Error", "Error", QMessageBox::Ok);
}

// Нажатие на кнопку Next
void Widget::on_btnNext_clicked()
{
    audioPlayer->stop();
    if(currentTrack < 0 || currentTrack >= trackListModel->rowCount() - 1) currentTrack = -1;
    const QModelIndex& i = trackListModel->index(++currentTrack);
    ui->trackListView->setCurrentIndex(i);
    QVariant var = trackListModel->data(i, Qt::DisplayRole);
    if(var.isValid())
    {
        setPlayMediaUpdateTrackName(var, 27);
    }
    else
        QMessageBox::information(this, "Error", "Error", QMessageBox::Ok);
}

// Двойное нажатие на список композиций
void Widget::on_trackListView_doubleClicked(const QModelIndex &index)
{
    audioPlayer->stop();
    currentTrack = index.row();
    QVariant var = trackListModel->data(index, Qt::DisplayRole);
    if(var.isValid())
    {
        setPlayMediaUpdateTrackName(var, 27);
    }
    else
        QMessageBox::information(this, "Error", "Error", QMessageBox::Ok);
}

void Widget::setPlayMediaUpdateTrackName(QVariant& var, int len)
{
    audioPlayer->setMedia(QUrl::fromLocalFile(dir->absoluteFilePath(var.toString())));
    ui->labelTrackName->setText(var.toString().length() <= len ? var.toString() : var.toString().left(len)+"...");
    audioPlayer->setPosition((qint64)20);
    on_btnPlay_clicked();
}

// Обработка статуса конца песни
// Автоматический переход на следующую композицию
void Widget::playerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::EndOfMedia)
    {
        audioPlayer->stop();
        //проверка на конец (если конец - остановить плеер)
        if(currentTrack >= trackListModel->rowCount() - 1) {
            this->setWindowIcon(*iconMain);
            trayIcon->showMessage("Track List is empty", "Select new directory with mp3 files", QSystemTrayIcon::Warning, 3500);
            return;
        }
        QVariant var = trackListModel->data(trackListModel->index(currentTrack + 1), Qt::DisplayRole);
        trayIcon->showMessage("Current Track", var.toString(), QSystemTrayIcon::Information, 3500);
        this->on_btnNext_clicked();
    }
}

// Нажитие кнопки, открытие диалога выбора папки с файлами *.mp3
void Widget::on_btnOpenDialogDir_clicked()
{
    QDir tempDir; // Временная директория
    QString path; // путь к директории
    path = QFileDialog::getExistingDirectory();
    if(path == "") // Если отмена или закрытие окна выбора директории
    {
        QMessageBox::information(this, "Information", "You did not select a folder", QMessageBox::Ok);
        return;
    }
    tempDir.setPath(path); // Установка пути во временную директорию
    *stringList = tempDir.entryList(*nameFilters); // Получение списка файлов (только *.mp3) из этой директории
    if(stringList->isEmpty()) // Если список пуст (Нет mp3 файлов в папке)
        QMessageBox::information(this, "Information", "Folder doesn't have mp3 files", QMessageBox::Ok);
    else
    {
        currentTrack = 0; // Устанавливаем текущую проигрываемую композицию
        dir->setPath(tempDir.path()); // Копируем путь из временную директории
        trackListModel->setStringList(*stringList); // Заносим список композиций в плейлист

        QVariant var = trackListModel->data(trackListModel->index(currentTrack), Qt::DisplayRole);
        if(var.isValid())
        {
            audioPlayer->setMedia(QUrl::fromLocalFile(dir->absoluteFilePath(var.toString())));
            ui->trackListView->setCurrentIndex(trackListModel->index(currentTrack));
            ui->labelTrackName->setText(var.toString().length() <= 27 ? var.toString() : var.toString().left(27)+"...");
        }
        else
            QMessageBox::information(this, "Error", "Error", QMessageBox::Ok);
    }
}

// Увеличение громкости
void Widget::volumeUp()
{
    int val = ui->VolumeSlider->value();
    if(val >= 95)
        return;
    ui->VolumeSlider->setValue(val + volumeStep);
    ui->VolumeSlider->valueChanged(val + volumeStep);
}

// Уменьшение громкости
void Widget::volumeDown()
{
    int val = ui->VolumeSlider->value();
    if(val <= 5)
        return;
    ui->VolumeSlider->setValue(val - volumeStep);
    ui->VolumeSlider->valueChanged(val - volumeStep);
}

// Обработка ошибок
void Widget::errorMessage(QMediaPlayer::Error e)
{
    QString str;
    if(e == QMediaPlayer::NoError)
        str = "No ERROR)";
    else if(e == QMediaPlayer::ResourceError)
        str = "A media resource couldn't be resolved.";
    else if(e == QMediaPlayer::FormatError)
    {
        str = QString("The format of a media resource isn't (fully) supported. ") +
              QString("Playback may still be possible, but without an audio or video component.");
    }
    else if(e == QMediaPlayer::ServiceMissingError)
        str = "A valid playback service was not found, playback cannot proceed.";
    else if(e == QMediaPlayer::AccessDeniedError)
        str = "There are not the appropriate permissions to play a media resource.";

    QMessageBox::critical(this, "Error", str ,QMessageBox::Ok);
}

void Widget::clickTray(QSystemTrayIcon::ActivationReason ac)
{
    if(ac == QSystemTrayIcon::Trigger)
    {
        QVariant var = trackListModel->data(trackListModel->index(currentTrack), Qt::DisplayRole);
        if(var.isValid())
            trayIcon->showMessage("Current Track", var.toString(), QSystemTrayIcon::Information, 3500);
        else
            trayIcon->showMessage("Error", "Select directory with mp3 files", QSystemTrayIcon::Critical, 3500);
    }
}
void Widget::trayMessageClick()
{
    // TODO
}
/*
 * regexp
 * ([^\\|\\\\\\/\\<\\>\\\"\\?\\*\\:]+)\\.mp3
 * #include <QRegExp>
 */
