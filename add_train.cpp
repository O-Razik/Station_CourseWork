#include "add_train.h"
#include "qmenu.h"
#include "ui_add_train.h"

add_train::add_train(std::vector<Station *>& table_, int index_, MainWindow& main_wind, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::add_train),
    index(index_),
    table(table_),
    main_window(main_wind)
{
    ui->setupUi(this);
}

add_train::~add_train()
{
    delete ui;
}

void add_train::closeEvent(QCloseEvent *event) {
    main_window.enable_window(true);
    this->~add_train();
}

void add_train::on_train_num_valueChanged(int arg1)
{
    for (int i = 0; i < table.size(); ++i){
        for (int j = 0; j < table[i]->trains_number(); ++j) {
            Train* train = table[i]->get_trains()[j].first;
            if(arg1 == train->get_id()){
                ui->label_error->setVisible(true);
                ui->label_error->setText("  ! Train with the same ID already exists");
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


void add_train::on_add_stop_clicked()
{
    if(ui->stop_name->text() != "")
        stops.push_back(std::make_pair(ui->stop_name->text(),ui->stop_time->time()));
    update_path_table();
}


void add_train::on_add_train_2_clicked()
{
    if(ui->train_dist->value() != 0 && ui->train_num->value() != 0 && stops.size() >= 2){
        Train *train = new Train(ui->train_num->value(),stops,ui->train_dist->value());
        for (int i = 0; i < table.size(); ++i){
            for (int j = 0; j < table[i]->trains_number(); ++j) {
                Train* t = table[i]->get_trains()[j].first;
                if(train->get_id() == t->get_id()){
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

        main_window.add_train_st(train);
        main_window.make_table(index);
        close();
    }
    else if(ui->train_dist->value() == 0 || ui->train_num->value() == 0){
        ui->label_error->setVisible(true);
        ui->label_error->setText("  !!! Train ID, distanse cannot be 0. Numder of stops should be >=2");
        ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
        return;
    }
    else if(stops.size() < 2){
        ui->label_error->setVisible(true);
        ui->label_error->setText("  !!! Numder of stops should be >= 2");
        ui->label_error->setStyleSheet("QLabel { background-color : red; color : black; }");
        return;
    }

}

void add_train::update_path_table(){
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


void add_train::on_path_table_customContextMenuRequested(const QPoint &pos)
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


void add_train::on_stop_name_customContextMenuRequested(const QPoint &pos)
{
    if (table.size() != 0) {
        QMenu menu(this);
        QAction *action;

        if(QString::fromStdString(table[index]->get_name()) != "-" &&
            QString::fromStdString(table[index]->get_name()) != "AllStations")
        {
            action = menu.addAction(QString::fromStdString(table[index]->get_name()));

            connect(action, &QAction::triggered, this, [this]() {
                ui->stop_name->setText(QString::fromStdString(table[index]->get_name()));
            });

            menu.addSeparator();
        }

        for (int i = 0; i < table.size(); i++) {
            if(QString::fromStdString(table[i]->get_name()) == "AllStations") continue;
            action = menu.addAction(QString::fromStdString(table[i]->get_name()));

            // Connect each action to a lambda with the correct value
            connect(action, &QAction::triggered, this, [this, i]() {
                ui->stop_name->setText(QString::fromStdString(table[i]->get_name()));
            });
        }

        // Show the context menu at the given position
        menu.exec(ui->stop_name->mapToGlobal(pos));
    }
}

