#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void showPrompt();
    void InitPrompt();    // Slot to show the prompt
    void refTable();  // Slot to show the prompt
    void OpenAddEdWindow(int index);
public slots:
    void onItemClicked(QTableWidgetItem* item);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
