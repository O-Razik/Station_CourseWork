#include "errorwindow.h"
#include "ui_errorwindow.h"

errorwindow::errorwindow(std::vector<QString>& message, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::errorwindow)
{
    ui->setupUi(this);
    for (int i = 0; i < message.size(); ++i) {
        ui->error_text->append(message[i]);
    }
    message.clear();
}

errorwindow::~errorwindow()
{
    delete ui;
}

void errorwindow::on_continue_button_clicked()
{
    close();
}

