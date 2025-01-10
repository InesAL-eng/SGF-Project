#include "viewelement.h"
#include "ui_viewelement.h"
#include "Struct.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "vdisk.bin";
int num_b;
int block_f;
FILE *vdisk_f;
FileData myFD;

void ViewElement::GetRecords() {
    vdisk_f = fopen(filename, "rb");
    if (vdisk_f == NULL) {
        perror("Error opening the vdisk file");
        return;
    }
    fread(&myFD, sizeof(myFD), 1, vdisk_f);
    num_b = myFD.n_blocks;
    block_f = myFD.f_blockage;
    int blockreq = ceil((float)filemeta.block_count / (float)block_f);
    Student **AllBuffers = (Student **)malloc(blockreq * sizeof(Student *));
    for (int i = 0; i < blockreq; ++i) {
        AllBuffers[i] = (Student *)malloc(block_f * sizeof(Student));
    }
    int filled = 0;

    if (filemeta.org_mode_intern == 0) {
        for (int block = 0; block < blockreq; ++block) {
            fseek(
                vdisk_f,
                sizeof(FileData) + num_b * sizeof(unsigned char) + num_b * sizeof(FileMetaData) +
                    filemeta.first_block * block_f * sizeof(Student) + block * block_f * sizeof(Student),
                SEEK_SET
                );
            fread(AllBuffers[block], sizeof(Student), block_f, vdisk_f);
        }
    } else {
        for (int block = filemeta.first_block; filled < blockreq;) {
            fseek(
                vdisk_f,
                sizeof(FileData) + num_b * sizeof(unsigned char) + num_b * sizeof(FileMetaData) +
                    block * block_f * sizeof(Student),
                SEEK_SET
                );
            fread(AllBuffers[filled], sizeof(Student), block_f, vdisk_f);

            block = AllBuffers[filled][0].nextBlock;
            filled++;
        }
    }

    ui->tableWidget->setRowCount(filemeta.block_count);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"Id", "Name (40 chars max)", "matricule (max char 20)", "section 1 char"});
    ui->tableWidget->verticalHeader()->setVisible(false);

    int rowIndex = 0;
    for (int buffer = 0; buffer < blockreq; ++buffer) {
        for (int indexBuffer = 0; indexBuffer < block_f; ++indexBuffer) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(AllBuffers[buffer][indexBuffer].id));
            QTableWidgetItem *item1 = new QTableWidgetItem(QString(AllBuffers[buffer][indexBuffer].full_name));
            QTableWidgetItem *item2 = new QTableWidgetItem(QString(AllBuffers[buffer][indexBuffer].mat));
            QTableWidgetItem *item3 = new QTableWidgetItem(QString(AllBuffers[buffer][indexBuffer].section));
            ui->tableWidget->setItem(rowIndex, 0, item);
            ui->tableWidget->setItem(rowIndex, 1, item1);
            ui->tableWidget->setItem(rowIndex, 2, item2);
            ui->tableWidget->setItem(rowIndex, 3, item3);
            rowIndex++;
        }
    }

    for (int i = 0; i < blockreq; ++i) {
        free(AllBuffers[i]);
    }
    free(AllBuffers);
}

ViewElement::ViewElement(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ViewElement) {
    ui->setupUi(this);
}

ViewElement::~ViewElement() {
    if (vdisk_f != NULL) {
        fclose(vdisk_f);
        vdisk_f = NULL;
    }

    // Reset global/static variables
    num_b = 0;
    block_f = 0;
    memset(&myFD, 0, sizeof(myFD));

    delete ui;

}

void ViewElement::on_pushButton_clicked() {
    QString idInputTxt = ui->lineEdit->text();
    if (!idInputTxt.isEmpty()) {
        ui->tableWidget->clear();
        vdisk_f = fopen(filename, "rb");
        if (vdisk_f == NULL) {
            perror("Error opening the vdisk file");
            return;
        }
        fread(&myFD, sizeof(myFD), 1, vdisk_f);
        num_b = myFD.n_blocks;
        block_f = myFD.f_blockage;
        int blockreq = ceil((float)filemeta.block_count / (float)block_f);
        Student **AllBuffers = (Student **)malloc(blockreq * sizeof(Student *));
        for (int i = 0; i < blockreq; ++i) {
            AllBuffers[i] = (Student *)malloc(block_f * sizeof(Student));
        }
        int filled = 0;

        if (filemeta.org_mode_intern == 0) {
            for (int block = 0; block < blockreq; ++block) {
                fseek(
                    vdisk_f,
                    sizeof(FileData) + num_b * sizeof(unsigned char) + num_b * sizeof(FileMetaData) +
                        filemeta.first_block * block_f * sizeof(Student) + block * block_f * sizeof(Student),
                    SEEK_SET
                    );
                fread(AllBuffers[block], sizeof(Student), block_f, vdisk_f);
            }
        } else {
            for (int block = filemeta.first_block; filled < blockreq;) {
                fseek(
                    vdisk_f,
                    sizeof(FileData) + num_b * sizeof(unsigned char) + num_b * sizeof(FileMetaData) +
                        block * block_f * sizeof(Student),
                    SEEK_SET
                    );
                fread(AllBuffers[filled], sizeof(Student), block_f, vdisk_f);
                block = AllBuffers[filled][0].nextBlock;
                filled++;
            }
        }

        ui->tableWidget->setRowCount(filemeta.block_count);
        ui->tableWidget->setColumnCount(4);
        ui->tableWidget->setHorizontalHeaderLabels({"Id", "Name (40 chars max)", "matricule (max char 20)", "section 1 char"});
        ui->tableWidget->verticalHeader()->setVisible(false);

        int rowIndex = 0;
        for (int buffer = 0; buffer < blockreq; ++buffer) {
            for (int indexBuffer = 0; indexBuffer < block_f; ++indexBuffer) {
                if(AllBuffers[buffer][indexBuffer].id==idInputTxt.toInt()){
                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(AllBuffers[buffer][indexBuffer].id));
                    QTableWidgetItem *item1 = new QTableWidgetItem(QString(AllBuffers[buffer][indexBuffer].full_name));
                    QTableWidgetItem *item2 = new QTableWidgetItem(QString(AllBuffers[buffer][indexBuffer].mat));
                    QTableWidgetItem *item3 = new QTableWidgetItem(QString(AllBuffers[buffer][indexBuffer].section));
                    ui->tableWidget->setItem(rowIndex, 0, item);
                    ui->tableWidget->setItem(rowIndex, 1, item1);
                    ui->tableWidget->setItem(rowIndex, 2, item2);
                    ui->tableWidget->setItem(rowIndex, 3, item3);
                    rowIndex++;
                    break;
                }
            }
        }

        for (int i = 0; i < blockreq; ++i) {
            free(AllBuffers[i]);
        }
        free(AllBuffers);

    } else {
        ui->tableWidget->clear();
        GetRecords();
    }
}
