#include "addeditdelwindow.h"
#include "ui_addeditdelwindow.h"
#include "MScontigue.h"
#include "ChainedMS.h"
#include "Struct.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <QInputDialog>

const char filename[] = "vdisk.bin";
int nb;
int bf;
FILE *vdisk;
FileData myFData;
FileMetaData filemeta;
Student **StdBuff;
void AddEditDelWindow::GetRcds(int fb2edit) {
    vdisk= fopen(filename, "rb+");
    if (vdisk == NULL) {
        perror("Error opening the vdisk file");
        return;
    }
    fread(&myFData, sizeof(myFData), 1, vdisk);
    nb = myFData.n_blocks;
    bf = myFData.f_blockage;
    fseek(vdisk,sizeof(FileData)+nb*sizeof(unsigned char),SEEK_SET);
    for (int metaInt = 0; metaInt < nb; ++metaInt) {
        fread(&filemeta,sizeof(FileMetaData),1,vdisk);
        if(filemeta.first_block==fb2edit){
            break;
        }
    }
    int blockreq = ceil((float)filemeta.block_count / (float)bf);
    StdBuff = (Student **)malloc(blockreq * sizeof(Student *));
    for (int i = 0; i < blockreq; ++i) {
        StdBuff[i] = (Student *)malloc(bf * sizeof(Student));
    }
    int filled = 0;

    if (filemeta.org_mode_intern == 0) {
        for (int block = 0; block < blockreq; ++block) {
            fseek(
                vdisk,
                sizeof(FileData) + nb * sizeof(unsigned char) + nb * sizeof(FileMetaData) +
                    filemeta.first_block * bf * sizeof(Student) + block * bf * sizeof(Student),
                SEEK_SET
                );
            fread(StdBuff[block], sizeof(Student), bf, vdisk);
        }
    } else {
        for (int block = filemeta.first_block; filled < blockreq;) {
            fseek(
                vdisk,
                sizeof(FileData) + nb * sizeof(unsigned char) + nb * sizeof(FileMetaData) +
                    block * bf * sizeof(Student),
                SEEK_SET
                );
            fread(StdBuff[filled], sizeof(Student), bf, vdisk);

            block = StdBuff[filled][0].nextBlock;
            filled++;
        }
    }

    ui->tableWidget_2->setRowCount(filemeta.block_count);
    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setHorizontalHeaderLabels({"Id", "Name (40 chars max)", "matricule (max char 20)", "section 1 char"});
    ui->tableWidget_2->verticalHeader()->setVisible(false);

    int rowIndex = 0;
    for (int buffer = 0; buffer < blockreq; ++buffer) {
        for (int indexBuffer = 0; indexBuffer < bf; ++indexBuffer) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(StdBuff[buffer][indexBuffer].id));
            QTableWidgetItem *item1 = new QTableWidgetItem(QString(StdBuff[buffer][indexBuffer].full_name));
            QTableWidgetItem *item2 = new QTableWidgetItem(QString(StdBuff[buffer][indexBuffer].mat));
            QTableWidgetItem *item3 = new QTableWidgetItem(QString(StdBuff[buffer][indexBuffer].section));
            ui->tableWidget_2->setItem(rowIndex, 0, item);
            ui->tableWidget_2->setItem(rowIndex, 1, item1);
            ui->tableWidget_2->setItem(rowIndex, 2, item2);
            ui->tableWidget_2->setItem(rowIndex, 3, item3);
            rowIndex++;
        }
    }
}
void AddEditDelWindow::reload(){
    ui->tabWidget->setCurrentIndex(indexCurrentTab);
}

AddEditDelWindow::AddEditDelWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AddEditDelWindow)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

AddEditDelWindow::~AddEditDelWindow()
{
    delete ui;
}
int stdCount;
void AddEditDelWindow::on_GenStudents_clicked()
{
    int StudAddCount=ui->spinBox->text().toInt();
    if(StudAddCount>0){
        stdCount=StudAddCount;
        ui->tableWidget->setRowCount(StudAddCount); // One row for the item
        ui->tableWidget->setColumnCount(4); // Three columns
        ui->tableWidget->setHorizontalHeaderLabels({"Id", "Name (40 chars max)", "matricule (max char 20)","section 1 char"});
        ui->tableWidget->verticalHeader()->setVisible(false);
        for (int row = 0; row <StudAddCount; ++row) {
            QTableWidgetItem *item  = new QTableWidgetItem(QString::number(row+1));
            QTableWidgetItem *item1 = new QTableWidgetItem(QString("name"));
            QTableWidgetItem *item2 = new QTableWidgetItem(QString("matricule"));
            QTableWidgetItem *item3 = new QTableWidgetItem(QString("section"));

            ui->tableWidget->setItem(row, 0, item);
            ui->tableWidget->setItem(row, 1, item1);
            ui->tableWidget->setItem(row, 2, item2);
            ui->tableWidget->setItem(row, 3, item3);
        }
    }

}

void AddEditDelWindow::DeltPrompt() {
    bool ok1;

    int idToDel = QInputDialog::getText(this, "delete", "Enter id of element to delete:", QLineEdit::Normal, QString(), &ok1).toInt();

    if (ok1) {
        int k=0;
        if(filemeta.org_mode_intern==0){
            fseek(
                vdisk,
                sizeof(FileData) + nb * sizeof(unsigned char) + nb * sizeof(FileMetaData) +
                    filemeta.first_block * bf * sizeof(Student) ,
                SEEK_SET
                );
            for (int elemCount = 0; elemCount < filemeta.block_count && elemCount<ui->tableWidget_2->rowCount(); ++elemCount) {
                if(ui->tableWidget_2->item(elemCount,0)->text().toInt()!=idToDel){
                    Student myCurrent;
                    myCurrent.id        = ui->tableWidget_2->item(elemCount, 0)->text().toInt();
                    strcpy(myCurrent.full_name ,ui->tableWidget_2->item(elemCount, 1)->text().toUtf8());
                    strcpy(myCurrent.mat       ,ui->tableWidget_2->item(elemCount, 2)->text().toUtf8());
                    myCurrent.section   = ui->tableWidget_2->item(elemCount, 3)->text().toUtf8()[0];
                    myCurrent.nextBlock = filemeta.first_block+elemCount+1;
                    fwrite(&myCurrent,sizeof(Student),1,vdisk);
                }else{
                    k++;
                    ui->tableWidget_2->removeRow(elemCount);
                    --elemCount;
                }
            }
            fseek(
                vdisk,
                sizeof(FileData) + nb * sizeof(unsigned char),
                SEEK_SET
                );
            FileMetaData tmpmeta[nb];
            fread(&tmpmeta,sizeof(FileMetaData),nb,vdisk);
            for (int metacount = 0; metacount < nb; ++metacount) {
                if(tmpmeta[metacount].first_block==filemeta.first_block){
                    tmpmeta[metacount].block_count-=k;
                    filemeta.block_count-=k;
                    break;
                }
            }
            fseek(
                vdisk,
                sizeof(FileData) + nb * sizeof(unsigned char),
                SEEK_SET
                );
            fwrite(tmpmeta,sizeof(FileMetaData),nb,vdisk);
            fclose(vdisk);
        }else{

            int blockreq = ceil((float)filemeta.block_count / (float)bf);
            int filledTb=0;
            int k=0;
            for (int i = 0; i < blockreq; ++i) {
                for (int j = 0; j < bf; ++j) {
                    if(k<ui->tableWidget_2->rowCount()){
                        if(idToDel!=StdBuff[i][j].id){
                            StdBuff[i][j].id        = ui->tableWidget_2->item(k, 0)->text().toInt();
                            strcpy(StdBuff[i][j].full_name ,ui->tableWidget_2->item(k, 1)->text().toUtf8());
                            strcpy(StdBuff[i][j].mat       ,ui->tableWidget_2->item(k, 2)->text().toUtf8());
                            StdBuff[i][j].section   = ui->tableWidget_2->item(k, 3)->text().toUtf8()[0];
                        }else{

                            StdBuff[i][j].id        = NULL;
                            strcpy(StdBuff[i][j].full_name ,"");
                            strcpy(StdBuff[i][j].mat       ,"");
                            StdBuff[i][j].section   = ' ';
                        }
                    }
                    k++;
                }
            }
            for (int block = filemeta.first_block; filledTb < blockreq;) {
                fseek(
                    vdisk,
                    sizeof(FileData) + nb * sizeof(unsigned char) + nb * sizeof(FileMetaData) +
                        block * bf * sizeof(Student),
                    SEEK_SET
                    );

                block = StdBuff[filledTb][0].nextBlock;
                fwrite(StdBuff[filledTb],sizeof(Student),bf,vdisk);
                filledTb++;
            }
            fclose(vdisk);
        }
        showMessageBox();
    } else {
        QMessageBox::warning(this, "deletd", "deletd Failed");
    }

}

void AddEditDelWindow::on_pushButton_clicked()
{

    Student mystd[stdCount];
    QString fname=ui->AddFileName->text();
    if(!fname.isEmpty()){
        for (int i = 0; i < stdCount; ++i) {
            QTableWidgetItem *item  = ui->tableWidget->item(i, 0); // Get the item at (row, col)
            QTableWidgetItem *item1 = ui->tableWidget->item(i, 1); // Get the item at (row, col)
            QTableWidgetItem *item2 = ui->tableWidget->item(i, 2); // Get the item at (row, col)
            QTableWidgetItem *item3 = ui->tableWidget->item(i, 3); // Get the item at (row, col)
            long Id = item->text().toLong();
            char name[40];
            strcpy(name, item1->text().toUtf8().data());
            char Mat[20];
            strcpy(Mat, item2->text().toUtf8().data());
            char section=item3->text().toUtf8().data()[0];

            mystd[i].id=Id;
            strcpy(mystd[i].full_name,name);
            strcpy(mystd[i].mat,Mat);
            mystd[i].section=section;
            mystd[i].nextBlock=0;
        }
        char filename[MAXFileName];
        strcpy(filename,fname.toUtf8().data());

        QString typeAdd=ui->comboBoxFileType->currentText();
        if(typeAdd==QString("Contigue")){
            ContigueAdd(filename,mystd,stdCount);
        }else{
            ChainedAdd(filename,mystd,stdCount);
        }
        showMessageBox();
    }


}


void AddEditDelWindow::on_pushButton_2_clicked()
{
    QString FFirstTxt=ui->lineEdit->text();
    if(!FFirstTxt.isEmpty()){
        ui->tableWidget_2->clear();
        GetRcds(FFirstTxt.toInt());
    }
    ui->pushButton_4->setEnabled(true);
}


void AddEditDelWindow::on_pushButton_3_clicked()
{
    if(filemeta.org_mode_intern==0){
        fseek(
            vdisk,
            sizeof(FileData) + nb * sizeof(unsigned char) + nb * sizeof(FileMetaData) +
                filemeta.first_block * bf * sizeof(Student) ,
            SEEK_SET
            );
        for (int elemCount = 0; elemCount < filemeta.block_count; ++elemCount) {
            Student myCurrent;
            myCurrent.id        = ui->tableWidget_2->item(elemCount, 0)->text().toInt();
            strcpy(myCurrent.full_name ,ui->tableWidget_2->item(elemCount, 1)->text().toUtf8());
            strcpy(myCurrent.mat       ,ui->tableWidget_2->item(elemCount, 2)->text().toUtf8());
            myCurrent.section   = ui->tableWidget_2->item(elemCount, 3)->text().toUtf8()[0];
            myCurrent.nextBlock = filemeta.first_block+elemCount+1;
            fwrite(&myCurrent,sizeof(Student),1,vdisk);
        }
        fclose(vdisk);
    }else{

        int blockreq = ceil((float)filemeta.block_count / (float)bf);
        int filledTb=0;
        int k=0;
        for (int i = 0; i < blockreq; ++i) {
            for (int j = 0; j < bf; ++j) {
                if(k<ui->tableWidget_2->rowCount()){
                    StdBuff[i][j].id        = ui->tableWidget_2->item(k, 0)->text().toInt();
                    strcpy(StdBuff[i][j].full_name ,ui->tableWidget_2->item(k, 1)->text().toUtf8());
                    strcpy(StdBuff[i][j].mat       ,ui->tableWidget_2->item(k, 2)->text().toUtf8());
                    StdBuff[i][j].section   = ui->tableWidget_2->item(k, 3)->text().toUtf8()[0];
                    StdBuff[i][j].section   = ui->tableWidget_2->item(k, 3)->text().toUtf8()[0];

                }
                k++;
            }
        }
        for (int block = filemeta.first_block; filledTb < blockreq;) {
            fseek(
                vdisk,
                sizeof(FileData) + nb * sizeof(unsigned char) + nb * sizeof(FileMetaData) +
                    block * bf * sizeof(Student),
                SEEK_SET
                );

            block = StdBuff[filledTb][0].nextBlock;
            fwrite(StdBuff[filledTb],sizeof(Student),bf,vdisk);
            filledTb++;
        }
        fclose(vdisk);
    }
    AddEditDelWindow::showMessageBox();

}


void AddEditDelWindow::on_pushButton_4_clicked()
{
    DeltPrompt();
}

