#ifndef ADD_CREATE_STATION_H
#define ADD_CREATE_STATION_H

#include <QDockWidget>

#include "mainwindow.h"
#include "ui_add_create_station.h"

namespace Ui {
class add_create_station;
}

class add_create_station : public QDockWidget
{
    Q_OBJECT

public:
    explicit add_create_station(MainWindow *main_wind, QWidget *parent = nullptr);
    ~add_create_station();

    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_station_name_editingFinished();

    void on_add_train_clicked();

    void on_add_stop_clicked();

    void update_path_table();

    void update_train_table();

    void on_train_num_valueChanged(int arg1);

    void on_add_station_clicked();

    void on_path_table_customContextMenuRequested(const QPoint &pos);

    void on_station_name_textEdited(const QString &arg1);

    void on_stop_name_customContextMenuRequested(const QPoint &pos);

    void on_train_table_customContextMenuRequested(const QPoint &pos);

private:
    Ui::add_create_station *ui;
    MainWindow* main_window;

    int mode = 1;
    Station *new_station;
    std::vector<std::pair<QString, QTime>> stops;
};

#endif // ADD_CREATE_STATION_H
