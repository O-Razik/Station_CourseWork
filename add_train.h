#ifndef ADD_TRAIN_H
#define ADD_TRAIN_H

#include <QDockWidget>
#include "mainwindow.h"

namespace Ui {
class add_train;
}

class add_train : public QDockWidget
{
    Q_OBJECT

public:
    explicit add_train(std::vector<Station *> &table_, int index_,MainWindow& main_wind, QWidget *parent = nullptr);
    ~add_train();

    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_train_num_valueChanged(int arg1);

    void on_add_stop_clicked();

    void on_add_train_2_clicked();

    void update_path_table();

    void on_path_table_customContextMenuRequested(const QPoint &pos);

    void on_stop_name_customContextMenuRequested(const QPoint &pos);

private:
    Ui::add_train *ui;

    int index;
    std::vector<Station *> &table;
    std::vector<std::pair<QString, QTime>> stops;

    Train* one;

    MainWindow &main_window;

};

#endif // ADD_TRAIN_H
