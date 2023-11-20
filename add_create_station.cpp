#include "add_create_station.h"
#include "qmenu.h"
#include "ui_add_create_station.h"
#include "mainwindow.h"

add_create_station::add_create_station(MainWindow *main_wind, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::add_create_station),
    main_window(main_wind)
{
    ui->setupUi(this);
    new_station = new Station;
    ui->label_error->setVisible(false);

    main_window->enable_window(false);
}


add_create_station::~add_create_station()
{
    if(mode) delete new_station;
    delete ui;
}

void add_create_station::closeEvent(QCloseEvent *event) {
    main_window->enable_window(true);
    this->~add_create_station();
}


void add_create_station::on_station_name_editingFinished()
{
    if(ui->station_name->text().length() >= 3)
    {
        new_station->set_name(ui->station_name->text().toStdString());
        ui->label_error->setVisible(false);
        ui->label_error->setText("");
        ui->label_error->setStyleSheet("QLabel { background-color : transparent; color : black; }");
    }
    else{
        ui->label_error->setVisible(true);
        ui->label_error->setText("  !!! Station name is to short");
        ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
    }
}


void add_create_station::on_add_train_clicked()
{
    Train *train = new Train(ui->train_num->value(),stops,ui->train_dist->value());
    if(main_window->station_amout() > 0){
        std::vector<unsigned int> check = main_window->stations_trains_id();
        for (int i = 0; i < check.size(); ++i) {
            if(train->get_id() == check[i]){
                ui->train_num->clear();
                delete train;
                ui->label_error->setVisible(true);
                ui->label_error->setText("  !!! Train with the same ID already exists");
                ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
                return;
            }
            else {
                ui->label_error->setVisible(false);
                ui->label_error->setText("");
                ui->label_error->setStyleSheet("QLabel { background-color : transparent; color : black; }");
            }
        }
    }
    QTime* begin_time = NULL;
    for(int i = 0; i< stops.size(); i++){
        if(stops[i].first == QString::fromStdString(new_station->get_name())){
            begin_time = &stops[i].second;
        }
    }
    if(begin_time == NULL) {
        delete train;
        return;
    }
    QTime end_time = train->get_stations().back().second;

    for(int i = 0; i< new_station->trains_number(); i++){
        if(train->get_id() == new_station->get_trains()[i].first->get_id() ){
            auto trains = new_station->get_trains();
            auto it = trains.begin() + i;

            if (it != trains.end()) {
                trains.erase(it);
                new_station->set_trains(trains);
            }
        }
    }
    new_station->add_train(std::make_pair(train, std::make_pair(*begin_time, end_time)));
    update_train_table();
}


void add_create_station::on_add_stop_clicked()
{
    if(ui->stop_name->text() != "")
        stops.push_back(std::make_pair(ui->stop_name->text(),ui->stop_time->time()));
    update_path_table();
}


void add_create_station::update_path_table(){
    ui->path_table->clear();

    int amount = stops.size();
    ui->path_table->setRowCount(2);
    ui->path_table->setColumnCount(amount);

    for (int i = 0; i < amount; ++i) {
        QTableWidgetItem *item;
        item = new QTableWidgetItem(stops[i].first);
        ui->path_table->setItem(0, i, item);
        item = new QTableWidgetItem(stops[i].second.toString("hh:mm"));
        ui->path_table->setItem(1, i, item);
    }
}


void add_create_station::update_train_table(){
    ui->train_table->clear();

    QStringList headerLabels;
    headerLabels << "№" << "Path" << "Distance";
    ui->train_table->setHorizontalHeaderLabels(headerLabels);

    int amount = new_station->trains_number();
    ui->train_table->setRowCount(amount);
    ui->train_table->setColumnCount(3);

    for (int i = 0; i < amount; ++i) {
        QTableWidgetItem *item;
        Train* train = new_station->get_trains()[i].first;
        item = new QTableWidgetItem(QString("%1").arg(train->get_id(), 4, 10, QChar('0')));
        ui->train_table->setItem(i, 0, item);

        QString path = "";
        for (int i = 0; i < train->get_stations().size(); ++i) {
            path += stops[i].first + " [" + stops[i].second.toString("hh:mm") + "] ";
        }

        item = new QTableWidgetItem(path);
        ui->train_table->setItem(i, 1, item);

        item = new QTableWidgetItem(QString::number(train->get_distance()) + " km");
        ui->train_table->setItem(i, 2, item);
    }
}


void add_create_station::on_train_num_valueChanged(int arg1)
{
    for (int i = 0; i < new_station->trains_number(); ++i) {
        Train* train = new_station->get_trains()[i].first;
        if(arg1 == train->get_id()){
            ui->label_error->setVisible(true);
            ui->label_error->setText("  ! Train with the same ID already exists");
            ui->label_error->setStyleSheet("QLabel { background-color : yellow; color : black; }");
            return;
        }
        else {
            ui->label_error->setVisible(false);
            ui->label_error->setText("");
            ui->label_error->setStyleSheet("QLabel { background-color : transparent; color : black; }");
        }
    }

    if(main_window->station_amout() > 0){
        std::vector<unsigned int> ids = main_window->stations_trains_id();
        for (int i = 0; i < ids.size(); ++i) {
            if(arg1 == ids[i]){
                ui->train_num->clear();
                ui->label_error->setVisible(true);
                ui->label_error->setText("  !!! Train with the same ID already exists");
                ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
                return;
            }
            else {
                ui->label_error->setVisible(false);
                ui->label_error->setText("");
                ui->label_error->setStyleSheet("QLabel { background-color : transparent; color : black; }");
            }
        }
    }
}


void add_create_station::on_add_station_clicked()
{
    if(new_station->trains_number() != 0){
        if(new_station->get_name().length() >= 3)
        {
            new_station->set_name(ui->station_name->text().toStdString());
            ui->label_error->setVisible(false);
            ui->label_error->setText("");
            ui->label_error->setStyleSheet("QLabel { background-color : transparent; color : black; }");
        }
        else{
            ui->label_error->setVisible(true);
            ui->label_error->setText("  !!! Station name is to short");
            ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
            return;
        }

        mode = 0;
        main_window->add_station(new_station);
        main_window->enable_window(true);
        close();
    }
}


void add_create_station::on_path_table_customContextMenuRequested(const QPoint &pos)
{
    if (ui->path_table->columnCount() != 0) {
        QModelIndex box = ui->path_table->indexAt(pos);
        int col = box.column();
        QMenu menu(this);

        QAction *deleteAction = menu.addAction("Delete");
        QAction *deleteALLAction = menu.addAction("Delete All");

        QAction *selectedAction = menu.exec(ui->path_table->viewport()->mapToGlobal(pos));
        if (selectedAction == deleteAction) {
            ui->path_table->removeColumn(col);
            stops.erase(stops.begin() + col);
        }
        if (selectedAction == deleteALLAction) {
            while (ui->path_table->columnCount() != 0) {
                ui->path_table->removeColumn(col);
                stops.erase(stops.begin() + col);
            }
        }
    }
}


void add_create_station::on_station_name_textEdited(const QString &arg1)
{
    if(main_window->station_amout() > 0){
        std::vector<QString> name = main_window->stations_qnames();
        for (int i = 0; i < name.size(); ++i) {
            if(arg1 == name[i]){
                ui->label_error->setVisible(true);
                ui->station_name->clear();
                ui->label_error->setText("Station with the same name already exists");
                ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
                return;
            }
            else {
                ui->label_error->setVisible(false);
                ui->label_error->setText("");
                ui->label_error->setStyleSheet("QLabel { background-color : transparent; color : black; }");
            }
        }
    }
}


void add_create_station::on_stop_name_customContextMenuRequested(const QPoint &pos)
{
    std::vector<QString> station_qnames = main_window->stations_qnames();
    if (station_qnames.size() != 0) {
        QMenu menu(this);
        QAction *action;

        if(QString::fromStdString(new_station->get_name()) != "-"){
            action = menu.addAction(ui->station_name->text());

            connect(action, &QAction::triggered, this, [this]() {
                ui->stop_name->setText(ui->station_name->text());
            });

            menu.addSeparator();
        }

        for (int i = 0; i < station_qnames.size(); i++) {
            if(station_qnames[i] == "AllStations") continue;
            action = menu.addAction(station_qnames[i]);

            // Connect each action to a lambda with the correct value
            connect(action, &QAction::triggered, this, [this, i, station_qnames]() {
                ui->stop_name->setText(station_qnames[i]);
            });
        }

        // Show the context menu at the given position
        menu.exec(ui->stop_name->mapToGlobal(pos));
    }
}


void add_create_station::on_train_table_customContextMenuRequested(const QPoint &pos)
{
    if (ui->train_table->rowCount() != 0){
        QModelIndex box = ui->train_table->indexAt(pos);
        int row = box.row();
        QMenu contextMenu(this);

        QMenu menu_id("№ " + QString::number(new_station->get_trains()[row].first->get_id()));
        QAction* action_change = menu_id.addAction("Change to" + ui->train_num->text());
        action_change->setWhatsThis("№ is changed to value from spinbox");

        connect(action_change, &QAction::triggered, this, [this, row]() {
            new_station->get_trains()[row].first->
                set_id(ui->train_num->value());
            update_train_table();
        });
        contextMenu.addMenu(&menu_id);

        QMenu menu_path("Path:");
        int train_num = static_cast<int>(new_station->get_trains()[row].first->get_stations().size());
        std::vector<QMenu*> station_menus;
        QAction* action_station;

        std::vector<std::pair<QString, QTime>> t = new_station->get_trains()[row].first->get_stations();

        for (int i = 0; i < train_num; i++) {
            QMenu* context_station = new QMenu(t[i].first + " [" + t[i].second.toString("hh:mm") + "]");

            action_station = context_station->addAction("Delete");

            connect(action_station, &QAction::triggered, this, [this, &t, i, row]() {
                std::vector<std::pair<QString, QTime>> sts;
                for (int j = 0; j < t.size(); ++j) {
                    if(j != i) sts.push_back(t[j]);
                }
                new_station->get_trains()[row].first->set_stations(sts);
                update_train_table();
            });

            station_menus.push_back(context_station);
            menu_path.addMenu(station_menus[i]);
        }
        contextMenu.addMenu(&menu_path);

        QMenu menu_dist(QString::number(new_station->get_trains()[row].first->get_distance()) + " km");
        action_change = menu_dist.addAction("Change to" + ui->train_dist->text());
        action_change->setWhatsThis("Distance is changed to value from spinbox");

        connect(action_change, &QAction::triggered, this, [this, row]() {
            new_station->get_trains()[row].first->
                set_distance(ui->train_dist->value());
            update_train_table();
        });
        contextMenu.addMenu(&menu_dist);

        QAction *selectedAction = contextMenu.exec(ui->train_table->viewport()->mapToGlobal(pos));
    }
}

