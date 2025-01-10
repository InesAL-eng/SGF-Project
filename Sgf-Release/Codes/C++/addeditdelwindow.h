#ifndef ADDEDITDELWINDOW_H
#define ADDEDITDELWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

namespace Ui {
class AddEditDelWindow;
}

class AddEditDelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddEditDelWindow(QWidget *parent = nullptr);
    ~AddEditDelWindow();
    int indexCurrentTab=0;

private slots:
    void on_GenStudents_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();
    void GetRcds(int fb2edit);
    void DeltPrompt();
    void showMessageBox() {
        QMessageBox::information(this, "Success", "Operation completed successfully!");
    }

private:
    Ui::AddEditDelWindow *ui;

public:
    int bFactor,numBlock;
public slots:
        void reload();

};

#endif // ADDEDITDELWINDOW_H
