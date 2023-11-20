#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "errorwindow.h"
#include "ui_mainwindow.h"
#include "add_create_station.h"
#include "add_train.h"
#include <set>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::update);
    updateTimer->start(800);

    connect(ui->train_table, &QTableWidget::customContextMenuRequested, this, &MainWindow::context_menu_table);
    ui->train_table->setContextMenuPolicy(Qt::CustomContextMenu);
}


MainWindow::~MainWindow()
{
    for(int i = 0; i < table.size(); i++){
        delete table[i];
    }
    table.clear();
    delete ui;
}


void MainWindow::update()
{
    int index = ui->station_choose->currentIndex();
    int size = ui->station_choose->count();
    if (!table.empty()) {
        emit ui->actionFirst_Station->enabledChanged(true);
        emit ui->actionPrev_Station->enabledChanged((index - 1) >= 0);
        emit ui->actionNex_Station->enabledChanged((index + 1) < size);
    } else {
        emit ui->actionFirst_Station->enabledChanged(false);
        emit ui->actionPrev_Station->enabledChanged((index - 1) >= 0);
        emit ui->actionNex_Station->enabledChanged((index + 1) < size);
    }
}


bool MainWindow::read_table_from_file(bool is_multp) {
    QString path = QFileDialog::getOpenFileName(
        nullptr, "Open File:" + QString(is_multp ? "Multiple Stations" : "One Station")
        , QDir::homePath(), "Text Files (*.txt);;All Files (*)"
    );

    if (path.isEmpty()) {
        return false;
    }

    // Clean up the existing stations in the table
    while (!table.empty()) {
        table.pop_back();
    }
    std::ifstream file((path.toStdWString()));

    if (!file.is_open()) {
        // Handle the case where the file could not be opened.
        return false;
    }

    Station* AllStation = new Station();

    try {
        file >> (*AllStation);
    }
    catch (std::vector<QString>& message) {
        errorwindow* error = new errorwindow(message);
        error->show();
        delete AllStation;
        message.clear();
        return false;
    }

    table.push_back(AllStation);

    if (is_multp) {
        multiple_stations();
    }

    return true;
}


void MainWindow::save_table_to_file(short mode){
    if(table.size() > 0){
        QString folderPath = QFileDialog::getExistingDirectory(
            nullptr, "Choose Destination Folder", QDir::homePath()
            );

        if (folderPath.isEmpty()) {
            return;  // User canceled the operation.
        }

        if (mode == 1) {
            QString filename = QFileDialog::getSaveFileName(
                nullptr, "Save File", folderPath, "Text Files (*.txt)"
                );

            if (filename.isEmpty()) {
                return;  // User canceled the operation.
            }

            QString fullPath = QDir(folderPath).filePath(filename);

            std::ofstream file(fullPath.toStdString()+".txt");

            if (!file.is_open()) {
                // Handle the case where the file could not be opened.
                return;
            }
            else file << *table[0];
            file.close();
        } else if (mode == 2) {
            int index = ui->station_choose->currentIndex();

            QString filename = QFileDialog::getSaveFileName(
                nullptr, "Save File", folderPath, "Text Files (*.txt)"
                );

            if (filename.isEmpty()) {
                return;  // User canceled the operation.
            }

            QString fullPath = QDir(folderPath).filePath(filename);


            std::ofstream file(fullPath.toStdString());

            if (!file.is_open()) {
                // Handle the case where the file could not be opened.
                return;
            }
            else file << *table[index];
            file.close();
        } else if (mode == 3) {
            Station* one = all_stations();
            QString fullPath = QDir(folderPath).filePath(
                QString::fromStdString(one->get_name())
            );
            std::ofstream file(fullPath.toStdString()+".txt");

            if (!file.is_open()) {
                // Handle the case where the file could not be opened.
                return;
            }
            else {
                file << *one;
                file.close();
            }
            delete one;  // Release memory after saving.
        } else if (mode == 4) {
            for (int i = 0; i < table.size(); ++i) {
                QString fullPath = QDir(folderPath).filePath(
                    QString::fromStdString(table[i]->get_name() + ".txt")
                    );
                std::ofstream file(fullPath.toStdString());

                if (!file.is_open()) {
                    // Handle the case where the file could not be opened.
                    return;
                }
                else file << *table[i];
                file.close();
            }
        }
    }
}


Station* MainWindow::all_stations()
{
    Station* all_station = new Station("AllStations");
    std::set<unsigned int> unique_train_ids;
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> all_trains;

    for (int i = 0; i < table.size(); ++i) {
        for (int j = 0; j < table[i]->trains_number(); ++j) {
            unsigned int train_id = table[i]->get_trains()[j].first->get_id();

            if (unique_train_ids.find(train_id) == unique_train_ids.end()) {
                unique_train_ids.insert(train_id);

                std::pair<Train*, std::pair<QTime, QTime>> train;
                train.second = std::make_pair(table[i]->get_trains()[j].second.first,
                                              table[i]->get_trains()[j].second.second);
                train.first = new Train(*(table[i]->get_trains()[j].first));
                all_trains.push_back(train);
            }
        }
    }

    all_station->set_trains(all_trains);
    return all_station;
}


int MainWindow::find_station_by_name(const QString &name) {
    for (int i = 0; i < table.size(); ++i) {
        if (table[i]->get_name().compare(name.toStdString()) == 0) {
            return i;
        }
    }
    return -1;
}


void MainWindow::multiple_stations(){
    for (int i = 0; i < table.size(); ++i) {
        for(int j = 0; j < table[i]->trains_number(); j++){
            Train* train = table[i]->get_trains()[j].first;
            for (int k = 0; k < train->get_stations().size(); ++k) {
                if (find_station_by_name(train->get_stations()[k].first) == -1)
                    add_existing_station(train->get_stations()[k].first);
            }
        }
    }
}


void MainWindow::add_existing_station(QString name) {
    Station* new_station = new Station(name.toStdString());

    // Loop through each train at the existing station
    for (int j = 0; j < table[0]->trains_number(); j++) {
        Train* train = table[0]->get_trains()[j].first;
        bool stationFound = false;
        int stationIndex = -1;

        // Check if the station name exists in the train's stations
        for (int k = 0; k < train->get_stations().size(); ++k) {
            if (train->get_stations()[k].first == name) {
                stationFound = true;
                stationIndex = k;
                break;
            }
        }

        if (stationFound) {
            // Calculate the time period for this station
            QTime beginTime = train->get_stations()[stationIndex].second;
            QTime endTime = train->get_stations().back().second;

            // Add the train to the new station
            new_station->add_train(std::make_pair(train, std::make_pair(beginTime, endTime)));
        }
    }

    // Add the new station to the table
    table.push_back(new_station);
}


void  MainWindow::create_table(Station* one){
    int count = ui->station_choose->count();

    if(count != 0) {
        ui->station_choose->clear();
    }

    for (int i = 0; i < table.size(); ++i) {
        ui->station_choose->addItem(QString::fromStdString(table[i]->get_name()));
    }

    int index = find_station_by_name(QString::fromStdString(one->get_name()));
    ui->station_choose->setCurrentIndex(index);
    make_table(index);
}


void MainWindow::add_station(Station* new_station) {
    // Add the new station to the table
    table.push_back(new_station);

    // Find the index of the new station
    int index = find_station_by_name(QString::fromStdString(new_station->get_name()));

    for (int i = 0; i < table.size(); ++i) {
        if(QString::fromStdString(table[i]->get_name()) == "AllStations"){
            for (int j = 0; j < table[index]->trains_number(); ++j) {
                auto current_train = table[index]->get_trains()[i];
                table[i]->add_train(std::make_pair(current_train.first,
                    std::make_pair(
                        current_train.first->get_stations()[0].second,
                        current_train.first->get_stations()[current_train.first->get_stations().size()-1].second
                        )
                    )
                );
            }
            break;
        }
    }

    // Iterate over the trains of the new station
    for (int i = 0; i < table[index]->trains_number(); ++i) {
        auto current_train = table[index]->get_trains()[i];

        // Iterate over the stations of the current train
        for (int j = 0; j < current_train.first->get_stations().size(); ++j) {
            // Iterate over all stations in the table
            for (int k = 0; k < table.size(); ++k) {
                // Check if the station in the current train exists in the table
                if (QString::fromStdString(table[k]->get_name()) == current_train.first->get_stations()[j].first
                        && k != index
                    )
                {
                    // Add the current train to the corresponding station in the table
                    table[k]->add_train(std::make_pair(current_train.first,
                        std::make_pair(
                            current_train.first->get_stations()[j].second,
                            current_train.first->get_stations()[current_train.first->get_stations().size()-1].second
                            )
                        )
                    );
                    break;
                }
            }
        }
    }

    // Update the UI for the new station
    create_table(table[index]);
}


void MainWindow::add_train_st(Train *new_train)
{
    // Iterate over all stations in the table
    for (int k = 0; k < table.size(); ++k) {
        // Check if the station in the current train exists in the table
        if (QString::fromStdString(table[k]->get_name()) == "AllStations")
        {
            // Add the current train to the corresponding station in the table
            table[k]->add_train(
                std::make_pair(new_train,
                    std::make_pair(
                        new_train->get_stations()[0].second,
                        new_train->get_stations()[new_train->get_stations().size()-1].second
                        )
                    )
                );
            break;
        }
    }

    // Iterate over the stations of the current train
    for (int j = 0; j < new_train->get_stations().size(); ++j) {
        // Iterate over all stations in the table
        for (int k = 0; k < table.size(); ++k) {
            // Check if the station in the current train exists in the table
            if (QString::fromStdString(table[k]->get_name()) == new_train->get_stations()[j].first)
            {
                // Add the current train to the corresponding station in the table
                table[k]->add_train(
                    std::make_pair(new_train,
                        std::make_pair(
                            new_train->get_stations()[j].second,
                            new_train->get_stations()[new_train->get_stations().size()-1].second
                            )
                        )
                    );
                break;
            }
        }
    }
}


void  MainWindow::make_table(int index){
    update_table(table[index]);
}


void MainWindow::update_table(Station* station) {
    if (!station) {
        return;  // Check if the station is valid
    }

    int amount = station->trains_number();
    ui->train_table->setRowCount(amount);  // Set the row count for the table
    ui->train_table->setColumnCount(7);

    for (int i = 0; i < amount; ++i) {
        Train* train = station->get_trains()[i].first;
        QTableWidgetItem *item;

        item = new QTableWidgetItem(QString("%1").arg(train->get_id(), 4, 10, QChar('0')));
        ui->train_table->setItem(i, 0, item);

        item = new QTableWidgetItem(train->get_begin().first);
        ui->train_table->setItem(i, 1, item);

        item = new QTableWidgetItem(train->get_end().first);
        ui->train_table->setItem(i, 2, item);

        QString station_list = " ";
        for (int j = 0; j < train->get_stations().size(); ++j) {
            station_list += QString(train->get_stations()[j].first) +
                            " [" + train->get_stations()[j].second.toString("hh:mm") + "] ";
            if (j + 1 != train->get_stations().size()) {
                station_list += "-> ";
            }
        }
        item = new QTableWidgetItem(station_list);
        ui->train_table->setItem(i, 3, item);

        item = new QTableWidgetItem(station->get_trains()[i].second.first.toString("hh:mm"));
        ui->train_table->setItem(i, 4, item);

        item = new QTableWidgetItem(
            (station->get_trains()[i].second.first == station->get_trains()[i].second.second) ?
            " " : station->get_trains()[i].second.second.toString("hh:mm")
        );
        ui->train_table->setItem(i, 5, item);

        item = new QTableWidgetItem(QString::number(train->get_distance()) + " km");
        ui->train_table->setItem(i, 6, item);
    }

    ui->label_train_number->setText(QString::number(station->trains_number()));
    ui->label_aver_speed->setText(QString::number(station->aver_train_speed()));
}


void MainWindow::on_action_One_Station_triggered()
{
    if(read_table_from_file(0)) create_table(table[0]);
}


void MainWindow::on_action_Multiple_Stations_triggered()
{
    if(read_table_from_file(1)) create_table(table[0]);
}


void MainWindow::on_station_choose_currentIndexChanged(int index)
{
    if(!table.empty() && index != -1) make_table(index);
}


void MainWindow::on_pm_timeslider_valueChanged(int value)
{
    ui->pm_time->setText("±" + QString::number(value) + (value >= 10 ? "" : " ") + " h");
}


void MainWindow::on_sort_speed_clicked()
{
    sort_by(&Station::sort_speed_trains);
}


void MainWindow::on_sort_distance_clicked()
{
    sort_by(&Station::sort_dict_trains);
}

void MainWindow::sort_by(void (Station::*sort)())
{
    if(!table.empty()){
        int index = ui->station_choose->currentIndex();
        (table[index]->*sort)();
        make_table(index);
    }
}


void MainWindow::time_check(
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>>
    (Station::*time_period)(QTime, QTime)
    ) {
    if (!table.empty()) {
        int index = ui->station_choose->currentIndex();

        QTime begin, end;
        QTime current_time = ui->train_time->time();
        if (ui->pm_timeslider->value() != 0) {
            int approcs_time = ui->pm_timeslider->value();
            begin = QTime(current_time.hour() - approcs_time, current_time.minute());
            end = QTime(current_time.hour() + approcs_time, current_time.minute());
        } else {
            begin = current_time;
            end = current_time;
        }

        Station* arrival_only = new Station("arrival_only", (table[index]->*time_period)(begin, end));
        update_table(arrival_only);
        delete arrival_only;
    }
}


void MainWindow::on_arrival_cheek_clicked()
{
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_departure_cheek_clicked()
{
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_station_choose_activated(int index)
{
    if(!table.empty()) make_table(index);
}


void MainWindow::context_menu_table(const QPoint &pos) {
    if (ui->train_table->rowCount() != 0) {
        QModelIndex box = ui->train_table->indexAt(pos);
        int row = box.row();
        int col = box.column();
        if (row >= 0) {
            switch (col) {
            case 0:
                context_menu_train(pos, row);
                break;
            case 1:
            case 2:
                context_menu_train_station(pos);
                break;
            case 3:
                context_menu_path(pos);
                break;
            case 4:
            case 5:
                context_menu_time(pos);
                break;
            case 6:
                context_menu_distance(pos);
                break;
            default:
                break;
            }
        }
        else{
            if(ui->station_choose->currentIndex() >= 0)
            {
                QMenu menu_add(this);
                QAction *add = menu_add.addAction("Add");
                connect(add, &QAction::triggered, this, [this]() {
                    add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
                    enable_window(false);
                    window->show();
                });

                menu_add.exec(ui->train_table->viewport()->mapToGlobal(pos));
            }
        }
    }
}


void MainWindow::context_menu_train_station(const QPoint &pos) {
    QModelIndex box = ui->train_table->indexAt(pos);
    int row = box.row();
    int col = box.column();
    QString name = ui->train_table->item(row, col)->text();

    QMenu contextMenu(this);
    Station* one = new Station(name.toStdString());
    int index = find_station_by_name(QString::fromStdString(one->get_name()));
    temporary_station(one);

    QAction *station_name = new QAction(name);
    station_name->setDisabled(true);
    contextMenu.addAction(station_name);

    contextMenu.addSeparator();

    QMenu menu_info("Info");
    QAction *info_trains_num = new QAction(
        "Trains: " + QString::number(one->trains_number())
        );
    info_trains_num->setDisabled(true);
    menu_info.addAction(info_trains_num);
    QAction *info_trains_aver_speed = new QAction(
        "AverSp: " + QString::number(one->aver_train_speed()) + "km/h"
        );
    info_trains_aver_speed->setDisabled(true);
    menu_info.addAction(info_trains_aver_speed);

    menu_info.addSeparator();
    QAction *add_station = new QAction;
    if (index != -1) {
        add_station->setText("Go to station");
    } else {
        add_station->setText("Add station");
    }
    menu_info.addAction(add_station);
    contextMenu.addMenu(&menu_info);

    contextMenu.addSeparator();

    QMenu menu_sort("Sort");
    QAction *sort_d = menu_sort.addAction("By distance");
    QAction *sort_s = menu_sort.addAction("By speed");
    contextMenu.addMenu(&menu_sort);

    contextMenu.addSeparator();

    QMenu menu_add("Add Train");
    QAction *add = menu_add.addAction("Add");
    connect(add, &QAction::triggered, this, [this]() {
        add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
        window->windowTitleChanged(ui->station_choose->currentText());
        enable_window(false);
        window->show();
    });
    contextMenu.addMenu(&menu_add);

    QAction *selectedAction = contextMenu.exec(ui->train_table->viewport()->mapToGlobal(pos));

    if (selectedAction == add_station) {
        if (index != -1) {
            ui->station_choose->setCurrentIndex(index);
        } else {
            add_existing_station(QString::fromStdString(one->get_name()));
            create_table(one);
        }
    } else if (selectedAction == sort_d) {
        sort_by(&Station::sort_dict_trains);
    } else if (selectedAction == sort_s) {
        sort_by(&Station::sort_speed_trains);
    }

    delete one;
}


void MainWindow::context_menu_distance(const QPoint &pos) {
    QMenu contextMenu(this);
    QMenu menu_sort("Sort");

    QAction *sort_d = menu_sort.addAction("By distance");
    QAction *sort_s = menu_sort.addAction("By speed");
    contextMenu.addMenu(&menu_sort);

    contextMenu.addSeparator();

    QMenu menu_add("Add Train");
    QAction *add = menu_add.addAction("Add");
    connect(add, &QAction::triggered, this, [this]() {
        add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
        window->windowTitleChanged(ui->station_choose->currentText());
        enable_window(false);
        window->show();
    });
    contextMenu.addMenu(&menu_add);

    QAction *selectedAction = contextMenu.exec(ui->train_table->viewport()->mapToGlobal(pos));
    if (selectedAction == sort_d) {
        sort_by(&Station::sort_dict_trains);
    } else if (selectedAction == sort_s) {
        sort_by(&Station::sort_speed_trains);
    }
}


void MainWindow::context_menu_path(const QPoint &pos) {
    QModelIndex box = ui->train_table->indexAt(pos);
    int row = box.row();
    QMenu contextMenu(this);

    QMenu menu_info("Stations:");
    std::vector<std::pair<QString, QTime>> stations =
        table[ui->station_choose->currentIndex()]->get_trains()[row].first->get_stations();
    int train_num = stations.size();

    std::vector<QMenu*> station_menus;
    std::vector<QAction*> action_station(train_num);
    std::vector<Station*> stations_data(train_num);
    std::vector<int> station_indices(train_num);

    for (int i = 0; i < train_num; i++) {
        QMenu* context_station = new QMenu(stations[i].first + " [" + stations[i].second.toString("hh:mm") + "]");
        stations_data[i] = new Station(stations[i].first.toStdString());
        station_indices[i] = find_station_by_name(stations[i].first);
        temporary_station(stations_data[i]);

        QAction *info_trains_num = new QAction("Trains: " + QString::number(stations_data[i]->trains_number()));
        info_trains_num->setDisabled(true);
        context_station->addAction(info_trains_num);

        QAction *info_trains_aver_speed = new QAction("AverSp: " + QString::number(stations_data[i]->aver_train_speed()) + "km/h");
        info_trains_aver_speed->setDisabled(true);
        context_station->addAction(info_trains_aver_speed);

        context_station->addSeparator();
        action_station[i] = new QAction;
        if (station_indices[i] != -1) {
            action_station[i]->setText("Go to station");
        } else {
            action_station[i]->setText("Add station");
        }
        context_station->addAction(action_station[i]);

        // Add the context_station to the station_menus vector
        station_menus.push_back(context_station);
        menu_info.addMenu(station_menus[i]);
    }
    contextMenu.addMenu(&menu_info);

    contextMenu.addSeparator();

    QMenu menu_sort("Sort");
    QAction *sort_d = menu_sort.addAction("By distance");
    QAction *sort_s = menu_sort.addAction("By speed");
    contextMenu.addMenu(&menu_sort);

    contextMenu.addSeparator();

    QMenu menu_add("Add Train");
    QAction *add = menu_add.addAction("Add");
    connect(add, &QAction::triggered, this, [this]() {
        add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
        window->windowTitleChanged(ui->station_choose->currentText());
        enable_window(false);
        window->show();
    });
    contextMenu.addMenu(&menu_add);

    QAction *selectedAction = contextMenu.exec(ui->train_table->viewport()->mapToGlobal(pos));

    for (int i = 0; i < train_num; i++) {
        if (selectedAction == action_station[i]) {
            if (station_indices[i] != -1) {
                ui->station_choose->setCurrentIndex(station_indices[i]);
            } else {
                add_existing_station(QString::fromStdString(stations_data[i]->get_name()));
                create_table(stations_data[i]);
            }
            break; // Exit the loop after handling this station
        }
    }

    if (selectedAction == sort_d) {
        sort_by(&Station::sort_dict_trains);
    } else if (selectedAction == sort_s) {
        sort_by(&Station::sort_speed_trains);
    }
}


void MainWindow::context_menu_time(const QPoint &pos) {
    QModelIndex box = ui->train_table->indexAt(pos);
    int row = box.row();
    int col = box.column();
    QMenu contextMenu(this);

    QString mode;
    QMenu menu_time;
    QAction *pm_time[7];
    QString time = ui->train_table->item(row,col)->text();
    QAction *time_ = new QAction(time);
    time_->setDisabled(true);
    contextMenu.addAction(time_);
    if (ui->train_table->item(row,col)->text() != " " ){
        contextMenu.addSeparator();
        if(col == 4) mode = "Arrival time";
        else if(col == 5) mode = "Departure time";
        menu_time.setTitle(mode);
        pm_time[0] = menu_time.addAction("This time");
        for (int i = 1; i < 7; ++i) {
            pm_time[i] = menu_time.addAction("± 0" + QString::number(i) + ":00");
        }
        contextMenu.addMenu(&menu_time);
    }
    else time_->setText("None");
    contextMenu.addSeparator();

    QMenu menu_sort("Sort");
    QAction *sort_d = menu_sort.addAction("By distance");
    QAction *sort_s = menu_sort.addAction("By speed");
    contextMenu.addMenu(&menu_sort);


    contextMenu.addSeparator();

    QMenu menu_add("Add Train");
    QAction *add = menu_add.addAction("Add");
    connect(add, &QAction::triggered, this, [this]() {
        add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
        window->windowTitleChanged(ui->station_choose->currentText());
        enable_window(false);
        window->show();
    });
    contextMenu.addMenu(&menu_add);

    QAction *selectedAction = contextMenu.exec(ui->train_table->viewport()->mapToGlobal(pos));

    if (ui->train_table->item(row,col)->text() != " " ){
        for (int i = 0; i < 7; ++i) {
            if (selectedAction == pm_time[i]){
                ui->train_time->setTime(QTime::fromString(time, "hh:mm"));
                ui->pm_timeslider->setValue(i);
                if(col == 4)time_check(&Station::arrive_trains_period);
                else if(col == 5)time_check(&Station::depart_trains_period);
            }
        }
    }

    if (selectedAction == sort_d) {
        sort_by(&Station::sort_dict_trains);
    } else if (selectedAction == sort_s) {
        sort_by(&Station::sort_speed_trains);
    }
}


void MainWindow::context_menu_train(const QPoint &pos, int &row) {
    QMenu contextMenu(this);

    QString id = ui->train_table->item(row,0)->text();
    QAction *ID = new QAction(id);
    ID->setDisabled(true);
    contextMenu.addAction(ID);

    contextMenu.addSeparator();

    QMenu menu_info_station_1("From");

    Station* one_1 = new Station(ui->train_table->item(row,1)->text().toStdString());
    int index_1 = find_station_by_name(QString::fromStdString(one_1->get_name()));
    temporary_station(one_1);

    QAction *station_name_1 = new QAction("Name: " + ui->train_table->item(row,1)->text());
    station_name_1->setDisabled(true);
    menu_info_station_1.addAction(station_name_1);

    QAction *info_station_trains_num_1 = new QAction(
        "Trains: " + QString::number(one_1->trains_number())
        );
    info_station_trains_num_1->setDisabled(true);
    menu_info_station_1.addAction(info_station_trains_num_1);
    QAction *info_trains_aver_speed = new QAction(
        "AverSp: " + QString::number(one_1->aver_train_speed()) + "km/h"
        );
    info_trains_aver_speed->setDisabled(true);
    menu_info_station_1.addAction(info_trains_aver_speed);

    menu_info_station_1.addSeparator();

    QAction *add_station_1 = new QAction;
    if (index_1 != -1) {
        add_station_1->setText("Go to station");
    } else {
        add_station_1->setText("Add station");
    }
    menu_info_station_1.addAction(add_station_1);
    contextMenu.addMenu(&menu_info_station_1);

    QMenu menu_info_station_2("To");

    Station* one_2 = new Station(ui->train_table->item(row,2)->text().toStdString());
    int index_2 = find_station_by_name(QString::fromStdString(one_2->get_name()));
    temporary_station(one_2);

    QAction *station_name_2 = new QAction("Name: " + ui->train_table->item(row,2)->text());
    station_name_2->setDisabled(true);
    menu_info_station_2.addAction(station_name_2);

    QAction *info_station_trains_num_2 = new QAction(
        "Trains: " + QString::number(one_2->trains_number())
        );
    info_station_trains_num_2->setDisabled(true);
    menu_info_station_2.addAction(info_station_trains_num_2);
    QAction *info_trains_aver_speed_2 = new QAction(
        "AverSp: " + QString::number(one_2->aver_train_speed()) + "km/h"
        );
    info_trains_aver_speed_2->setDisabled(true);
    menu_info_station_2.addAction(info_trains_aver_speed_2);

    menu_info_station_2.addSeparator();
    QAction *add_station_2 = new QAction;
    if (index_2 != -1) {
        add_station_2->setText("Go to station");
    } else {
        add_station_2->setText("Add station");
    }
    menu_info_station_2.addAction(add_station_2);
    contextMenu.addMenu(&menu_info_station_2);

    contextMenu.addSeparator();

    QMenu menu_path("Path:");
    std::vector<std::pair<QString, QTime>> stations =
        table[ui->station_choose->currentIndex()]->get_trains()[row].first->get_stations();
    int train_num = stations.size();

    std::vector<QMenu*> station_menus;
    std::vector<QAction*> action_station(train_num);
    std::vector<Station*> stations_data(train_num);
    std::vector<int> station_indices(train_num);

    for (int i = 0; i < train_num; i++) {
        QMenu* context_station = new QMenu(stations[i].first + " [" + stations[i].second.toString("hh:mm") + "]");
        stations_data[i] = new Station(stations[i].first.toStdString());
        station_indices[i] = find_station_by_name(stations[i].first);
        temporary_station(stations_data[i]);

        QAction *info_trains_num = new QAction("Trains: " + QString::number(stations_data[i]->trains_number()));
        info_trains_num->setDisabled(true);
        context_station->addAction(info_trains_num);

        QAction *info_trains_aver_speed = new QAction("AverSp: " + QString::number(stations_data[i]->aver_train_speed()) + "km/h");
        info_trains_aver_speed->setDisabled(true);
        context_station->addAction(info_trains_aver_speed);

        context_station->addSeparator();
        action_station[i] = new QAction;
        if (station_indices[i] != -1) {
            action_station[i]->setText("Go to station");
        } else {
            action_station[i]->setText("Add station");
        }
        context_station->addAction(action_station[i]);

        // Add the context_station to the station_menus vector
        station_menus.push_back(context_station);
        menu_path.addMenu(station_menus[i]);
    }
    contextMenu.addMenu(&menu_path);

    contextMenu.addSeparator();

    QMenu menu_time("Time");
    QMenu menu_time_arr("Arrival");

    QString time_arr = ui->train_table->item(row,4)->text();
    QAction *time_arr_action = new QAction(time_arr);
    time_arr_action->setDisabled(true);
    menu_time_arr.addAction(time_arr_action);

    menu_time_arr.addSeparator();

    QAction *pm_time_arr[7];
    pm_time_arr[0] =  menu_time_arr.addAction("This time");
    for (int i = 1; i < 7; ++i) {
        pm_time_arr[i] =  menu_time_arr.addAction("± 0" + QString::number(i) + ":00");
    }

    menu_time.addMenu(&menu_time_arr);

    QMenu menu_time_depr("Departure");

    QString time_depr = ui->train_table->item(row,5)->text();
    QAction *time_depr_action = new QAction(time_depr);
    time_depr_action->setDisabled(true);
    QAction *pm_time_depr[7];
    if (ui->train_table->item(row,5)->text() != " "){
        menu_time_depr.addAction(time_depr_action);
        menu_time_depr.addSeparator();

        pm_time_depr[0] =  menu_time_depr.addAction("This time");
        for (int i = 1; i < 7; ++i) {
            pm_time_depr[i] =  menu_time_depr.addAction("± 0" + QString::number(i) + ":00");
        }
    }
    else {
        time_depr_action->setText("None");
        time_depr_action->setWhatsThis("Choosen station is end of the road for this train");
        menu_time_depr.addAction(time_depr_action);
    }

    menu_time.addMenu(&menu_time_depr);

    contextMenu.addMenu(&menu_time);

    contextMenu.addSeparator();

    QMenu menu_sort("Sort");
    QAction *sort_d = menu_sort.addAction("By distance");
    QAction *sort_s = menu_sort.addAction("By speed");
    contextMenu.addMenu(&menu_sort);

    contextMenu.addSeparator();

    QMenu menu_add("Add Train");
    QAction *add = menu_add.addAction("Add");
    connect(add, &QAction::triggered, this, [this]() {
        add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
        window->windowTitleChanged(ui->station_choose->currentText());
        enable_window(false);
        window->show();
    });
    contextMenu.addMenu(&menu_add);

    QAction *selectedAction = contextMenu.exec(ui->train_table->viewport()->mapToGlobal(pos));

    if (selectedAction == add_station_1) {
        if (index_1 != -1) {
            ui->station_choose->setCurrentIndex(index_1);
        } else {
            add_existing_station(QString::fromStdString(one_1->get_name()));
            create_table(one_1);
        }
    } else if (selectedAction == add_station_2) {
        if (index_2 != -1) {
            ui->station_choose->setCurrentIndex(index_2);
        } else {
            add_existing_station(QString::fromStdString(one_2->get_name()));
            create_table(one_2);
        }
    }
    for (int i = 0; i < train_num; i++) {
        if (selectedAction == action_station[i]) {
            if (station_indices[i] != -1) {
                ui->station_choose->setCurrentIndex(station_indices[i]);
            } else {
                add_existing_station(QString::fromStdString(stations_data[i]->get_name()));
                create_table(stations_data[i]);
            }
            break;
        }
    }
    for (int i = 0; i < 7; ++i) {
        if (selectedAction == pm_time_arr[i]){
            ui->train_time->setTime(QTime::fromString(time_arr, "hh:mm"));
            ui->pm_timeslider->setValue(i);
            time_check(&Station::arrive_trains_period);
            break;
        }
        if (ui->train_table->item(row,5)->text() != "" && selectedAction == pm_time_depr[i]){
            ui->train_time->setTime(QTime::fromString(time_arr, "hh:mm"));
            ui->pm_timeslider->setValue(i);
            time_check(&Station::depart_trains_period);
            break;
        }
    }
    if (selectedAction == sort_d) {
        sort_by(&Station::sort_dict_trains);
    } else if (selectedAction == sort_s) {
        sort_by(&Station::sort_speed_trains);
    }

    delete one_1, delete one_2;
}


void MainWindow::temporary_station(Station* one) {
    for (int i = 0; i < table.size(); ++i) {
        for (int j = 0; j < table[i]->trains_number(); j++) {
            Train* train = table[i]->get_trains()[j].first;
            bool stationFound = false;
            int stationIndex = -1;

            // Check if the station name exists in the train's stations
            for (int k = 0; k < train->get_stations().size(); ++k) {
                if (train->get_stations()[k].first == QString::fromStdString(one->get_name())) {
                    stationFound = true;
                    stationIndex = k;
                    break;
                }
            }

            if (stationFound) {
                bool check = 1;
                for (int m = 0; m < one->trains_number(); ++m) {
                    if(train->get_id() == one->get_trains()[m].first->get_id()){
                        check = 0;
                        break;
                    }
                }

                if(check){
                    Train* new_train = new Train(*train);
                    QTime beginTime = train->get_stations()[stationIndex].second;
                    QTime endTime = (stationIndex + 1 < train->get_stations().size()) ?
                                        train->get_stations()[stationIndex + 1].second :
                                        train->get_stations().back().second;

                    one->add_train(std::make_pair(new_train, std::make_pair(beginTime, endTime)));
                }
            }
        }
    }
}


int MainWindow::station_amout(){
    return static_cast<int>(table.size());
}


std::vector<unsigned int> MainWindow::stations_trains_id() {
    std::vector<unsigned int> trains_ids;
    if (table.size() > 0) {
        trains_ids.push_back(table[0]->get_trains()[0].first->get_id());
        for (int i = 0; i < table.size(); ++i) {
            for (int j = 0; j < table[i]->trains_number(); ++j) {
                auto t = table[i]->get_trains()[j].first;
                if (std::find(trains_ids.begin(), trains_ids.end(), t->get_id()) == trains_ids.end()) {
                    trains_ids.push_back(t->get_id());
                }
            }
        }
    }
    return trains_ids;
}


std::vector<QString> MainWindow::stations_qnames(){
    std::vector<QString> qnames;
    for (int i = 0; i < table.size(); ++i) {
        qnames.push_back(QString::fromStdString(table[i]->get_name()));
    }
    return qnames;
}


void MainWindow::enable_window(bool enbl){
    qDebug() << "Before setEnabled()";
    setEnabled(enbl);
    qDebug() << "After setEnabled()";
}


void MainWindow::on_actionSave_First_triggered()
{
    save_table_to_file(1);
}


void MainWindow::on_actionSave_Current_triggered()
{
    save_table_to_file(2);
}


void MainWindow::on_actionSave_One_File_triggered()
{
    save_table_to_file(3);
}


void MainWindow::on_actionSave_Multiple_Files_triggered()
{
    save_table_to_file(4);
}


void MainWindow::on_actionFirst_Station_triggered()
{
    if(!table.empty()) ui->station_choose->setCurrentIndex(0);
}


void MainWindow::on_actionPrev_Station_triggered()
{
    if(!table.empty()){
        int index = ui->station_choose->currentIndex() - 1;
        if(index >= 0) ui->station_choose->setCurrentIndex(index);
    }
}


void MainWindow::on_actionNex_Station_triggered()
{
    if(!table.empty()){
        int index = ui->station_choose->currentIndex() + 1;
        if(index < table.size()) ui->station_choose->setCurrentIndex(index);
    }
}


void MainWindow::on_actionBy_Speed_triggered()
{
    sort_by(&Station::sort_speed_trains);
}


void MainWindow::on_actionDistance_triggered()
{
    sort_by(&Station::sort_dict_trains);
}


void MainWindow::on_action_p1_hour_triggered()
{
    ui->train_time->setTime(ui->train_time->time().addSecs(3600));
}


void MainWindow::on_action_p30_minutes_triggered()
{
    ui->train_time->setTime(ui->train_time->time().addSecs(1800));
}


void MainWindow::on_action_m1_hour_triggered()
{
    ui->train_time->setTime(ui->train_time->time().addSecs(-3600));
}


void MainWindow::on_action_m30_minutes_triggered()
{
    ui->train_time->setTime(ui->train_time->time().addSecs(-1800));
}


void MainWindow::on_actionRefresh_Table_triggered()
{
    int index = ui->station_choose->currentIndex();
    if(!table.empty()) make_table(index);
}


void MainWindow::on_actionThis_Time_arr_triggered()
{
    ui->pm_timeslider->setValue(0);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_action_1_h_arr_triggered()
{
    ui->pm_timeslider->setValue(1);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_action_2_h_arr_triggered()
{
    ui->pm_timeslider->setValue(2);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_action_3_h_arr_triggered()
{
    ui->pm_timeslider->setValue(3);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_action_4_h_arr_triggered()
{
    ui->pm_timeslider->setValue(4);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_action_5_h_arr_triggered()
{
    ui->pm_timeslider->setValue(5);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_action_6_h_arr_triggered()
{
    ui->pm_timeslider->setValue(6);
    time_check(&Station::arrive_trains_period);
}


void MainWindow::on_actionThis_Time_dep_triggered()
{
    ui->pm_timeslider->setValue(0);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_action_1_h_dep_triggered()
{
    ui->pm_timeslider->setValue(1);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_action_2_h_dep_triggered()
{
    ui->pm_timeslider->setValue(2);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_action_3_h_dep_triggered()
{
    ui->pm_timeslider->setValue(3);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_action_4_h_dep_triggered()
{
    ui->pm_timeslider->setValue(4);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_action_5_h_dep_triggered()
{
    ui->pm_timeslider->setValue(5);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_action_6_h_dep_triggered()
{
    ui->pm_timeslider->setValue(6);
    time_check(&Station::depart_trains_period);
}


void MainWindow::on_actionCreate_triggered()
{
    add_create_station* window = new add_create_station(this);
    window->show();

    setEnabled(false);
}


void MainWindow::on_actionAdd_Train_triggered()
{
    if(ui->station_choose->currentIndex() >= 0) {
        add_train* window = new add_train(table, ui->station_choose->currentIndex(),*this);
        enable_window(false);
        window->show();
    }
}

