/*
#include <QTest>
#include "unit_test.h"
#include "widget.h"
#include "ui_widget.h"
unit_test::unit_test(QObject *parent) :
    QObject(parent)
{
    // Запись в файл
    freopen("testing.log", "w", stdout);
}
void unit_test::testCountTrackInPlaylist()
{
    // Проверка количества треков в плей-листе в выбранной папке
    w.on_btnOpenDialogDir_clicked();
    QCOMPARE(w.trackListModel->rowCount(), 5);
}
void unit_test::testNameTrackLabel()
{
    // Проверка GUI Название композиции в label
    w.on_trackListView_doubleClicked(w.trackListModel->index(1));
    QCOMPARE((w.ui)->labelTrackName->text(), QString("Avicii - Levels.mp3"));
}
*/
