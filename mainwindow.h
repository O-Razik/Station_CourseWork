#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "station.h"
#include "train.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void update();

    bool read_table_from_file(bool is_multp);

    void save_table_to_file(short mode);

    Station* all_stations();

    void multiple_stations();

    void add_existing_station(QString name);

    void create_table(Station *one);

    void add_station(Station *new_station);

    void add_train_st(Train* new_train);

    void make_table(int index);

    void update_table(Station* station);

    void time_check(std::vector<std::pair<Train *, std::pair<QTime, QTime> > >
        (Station::*time_period)(QTime, QTime));

    void sort_by(void(Station::*sort)());

    int find_station_by_name(const QString &name);

    void temporary_station(Station* one);

    int station_amout();

    std::vector<unsigned int> stations_trains_id();

    std::vector<QString> stations_qnames();

    void enable_window(bool enbl);

private slots:
    void on_action_One_Station_triggered();

    void on_station_choose_currentIndexChanged(int index);

    void on_action_Multiple_Stations_triggered();

    void on_pm_timeslider_valueChanged(int value);

    void on_sort_speed_clicked();

    void on_sort_distance_clicked();

    void on_arrival_cheek_clicked();

    void on_departure_cheek_clicked();

    void on_station_choose_activated(int index);

    void context_menu_table(const QPoint &pos);

    void context_menu_train_station(const QPoint &pos);

    void context_menu_distance(const QPoint &pos);

    void context_menu_path(const QPoint &pos);

    void context_menu_time(const QPoint &pos);

    void context_menu_train(const QPoint &pos, int &row);

    void on_actionSave_First_triggered();

    void on_actionSave_Current_triggered();

    void on_actionSave_One_File_triggered();

    void on_actionSave_Multiple_Files_triggered();

    void on_actionFirst_Station_triggered();

    void on_actionPrev_Station_triggered();

    void on_actionNex_Station_triggered();

    void on_actionBy_Speed_triggered();

    void on_actionDistance_triggered();

    void on_action_p1_hour_triggered();

    void on_action_p30_minutes_triggered();

    void on_action_m1_hour_triggered();

    void on_action_m30_minutes_triggered();

    void on_actionRefresh_Table_triggered();

    void on_actionThis_Time_arr_triggered();

    void on_action_1_h_arr_triggered();

    void on_action_2_h_arr_triggered();

    void on_action_3_h_arr_triggered();

    void on_action_4_h_arr_triggered();

    void on_action_5_h_arr_triggered();

    void on_action_6_h_arr_triggered();

    void on_actionThis_Time_dep_triggered();

    void on_action_1_h_dep_triggered();

    void on_action_2_h_dep_triggered();

    void on_action_3_h_dep_triggered();

    void on_action_4_h_dep_triggered();

    void on_action_5_h_dep_triggered();

    void on_action_6_h_dep_triggered();

    void on_actionCreate_triggered();

    void on_actionAdd_Train_triggered();

private:
    std::vector<Station*> table;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
