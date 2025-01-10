#ifndef VIEWELEMENT_H
#define VIEWELEMENT_H

#include <QMainWindow>
#include "Struct.h"

namespace Ui {
class ViewElement;
}

class ViewElement : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewElement(QWidget *parent = nullptr);
    ~ViewElement();

private:
    Ui::ViewElement *ui;
public :
    FileMetaData filemeta;
public slots:
    void GetRecords();
private slots:
    void on_pushButton_clicked();
};

#endif // VIEWELEMENT_H
