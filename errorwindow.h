#ifndef ERRORWINDOW_H
#define ERRORWINDOW_H

#include <QDockWidget>

namespace Ui {
class errorwindow;
}

class errorwindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit errorwindow(std::vector<QString> &message, QWidget *parent = nullptr);
    ~errorwindow();

private slots:
    void on_continue_button_clicked();

private:
    Ui::errorwindow *ui;
};

#endif // ERRORWINDOW_H
