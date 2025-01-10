#include "mainwindow.h"
#include "addeditdelwindow.h"
#include "viewelement.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include "Struct.h"
#include "initVdisk.h"
#include "suppression.h"
#include "compactage.h"
const char filename[] = "vdisk.bin";
int num_blocks;
int block_factor;
int TbCount;
FILE *vdisk_file;
FileData myfdata;
unsigned char *block_status;
FileMetaData *file_table;
FileMetaData myfMeta;
int i, max_files;
bool exist;

AddEditDelWindow *manageWin;
ViewElement *viewElem;

void read_org_part(const char *filename) {
    vdisk_file = fopen(filename, "rb");
    if (vdisk_file == NULL) {
        perror("Error opening the vdisk file");
        return;
    }

    fread(&myfdata, sizeof(myfdata), 1, vdisk_file);
    num_blocks = myfdata.n_blocks;
    block_factor = myfdata.f_blockage;

    block_status = (unsigned char *)malloc(num_blocks * sizeof(unsigned char));
    if (block_status == NULL) {
        perror("Memory allocation failed for block status table");
        fclose(vdisk_file);
        return;
    }

    fread(block_status, sizeof(unsigned char), num_blocks, vdisk_file);
    file_table = (FileMetaData *)malloc(num_blocks * sizeof(FileMetaData));
    fread(file_table, sizeof(FileMetaData), num_blocks, vdisk_file);
}

void MainWindow::onItemClicked(QTableWidgetItem *item) {
    if(check_vdisk(filename)){
        read_org_part(filename);
    }
    int row = item->row();
    int FirstBlock = ui->tableWidget->item(row, 2)->text().toInt();

    for (int i = 0; i < num_blocks; ++i) {
        if (file_table[i].first_block == FirstBlock) {
            viewElem = new ViewElement(this);
            viewElem->filemeta.first_block = file_table[i].first_block;
            viewElem->filemeta.block_count = file_table[i].block_count;
            strcpy(viewElem->filemeta.file_name, file_table[i].file_name);
            viewElem->filemeta.num_blocks = file_table[i].num_blocks;
            viewElem->filemeta.org_mode_extern = file_table[i].org_mode_extern;
            viewElem->filemeta.org_mode_intern = file_table[i].org_mode_intern;

            break;
        }
    }
    viewElem->GetRecords();
    viewElem->show();
}

void MainWindow::refTable() {
    exist = check_vdisk(filename);
    if (exist) {
        read_org_part(filename);
    }

    ui->tableWidget->setRowCount(myfdata.n_blocks);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"File Name", "File Size", "First Block"});
    ui->tableWidget->verticalHeader()->setVisible(false);

    for (int row = 0; row < num_blocks; ++row) {
        QTableWidgetItem *item = new QTableWidgetItem(file_table[row].file_name);
        QTableWidgetItem *item1 = new QTableWidgetItem(
            QString::number(file_table[row].block_count) + " records \\ " +
            QString::number(file_table[row].num_blocks) + " blocks");
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(file_table[row].first_block));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, item);
        ui->tableWidget->setItem(row, 1, item1);
        ui->tableWidget->setItem(row, 2, item2);
        ui->tableWidget->setRowHeight(row, 80);
    }

    QObject::disconnect(ui->tableWidget, &QTableWidget::itemClicked, this, &MainWindow::onItemClicked);
    QObject::connect(ui->tableWidget, &QTableWidget::itemClicked, this, &MainWindow::onItemClicked);
}

void MainWindow::showPrompt() {
    bool ok;
    QString text = QInputDialog::getText(this, "Prompt", "Enter file first block to delete:", QLineEdit::Normal, QString(), &ok);

    if (ok && !text.isEmpty()) {
        delete_file("vdisk.bin", text.toInt());
        QMessageBox::information(this, "Input", "Deleted !!!");
        refTable();
    } else {
        QMessageBox::warning(this, "Cancelled", "No input provided.");
    }
}

void MainWindow::InitPrompt() {
    bool AlreadyExist = check_vdisk(filename);
    bool ok1;
    bool ok2;
    int BlockFactor, BlockCount = 0;

    if (!AlreadyExist) {
        do {
            BlockCount = QInputDialog::getText(this, "Init", "Enter number of Blocks in your vdisk:", QLineEdit::Normal, QString(), &ok1).toInt();
            BlockFactor = QInputDialog::getText(this, "Init", "Enter Block Factor:", QLineEdit::Normal, QString(), &ok2).toInt();
        } while (BlockCount <= 0 || BlockFactor <= 0 || BlockFactor >= BlockCount);

        if (ok1 && ok2) {
            init(BlockCount, BlockFactor);
            QMessageBox::information(this, "Input", "Initialisation Done !!!");
            refTable();
        } else {
            QMessageBox::warning(this, "Init", "Init Failed");
        }
    } else {
        QMessageBox::warning(this, "Init", "vdisk already exists");
    }
}

void MainWindow::OpenAddEdWindow(int index) {
    if(check_vdisk(filename)){
        read_org_part(filename);
    }

    manageWin = new AddEditDelWindow(this);
    manageWin->indexCurrentTab=index;
    manageWin->reload();
    manageWin->bFactor = block_factor;
    manageWin->numBlock = num_blocks;
    manageWin->show();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    exist = check_vdisk(filename);
    if (exist) {
        read_org_part(filename);
    }

    ui->setupUi(this);
    ui->tableWidget->setRowCount(myfdata.n_blocks);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"File Name", "File Size", "First Block"});
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #3498db;"   // Header background color
        "    color: white;"                // Header text color
        "    font-weight: bold;"           // Bold header text
        "    padding-left: 10px;"
        "    padding-right: 10px;"
        "    padding-top: 30px;"
        "    padding-bottom: 30px;"        // Header padding
        "    border: 1px solid #2980b9;"   // Header border color
        "}"
        "QTableWidget::item {"
        "    padding-left: 10px;"
        "    padding-right: 10px;"
        "    padding-top: 30px;"
        "    padding-bottom: 30px;"        // Item padding
        "    background-color: #111376;"   // Item background color
        "}"
        );
    ui->pushButton->setStyleSheet(
        "QPushButton {"
        "    color: white;"                 // Font color
        "    background-color: #3498db;"   // Button background color
        "    border: 2px solid #2980b9;"   // Border color
        "    border-radius: 5px;"          // Rounded corners
        "    padding: 5px;"                // Button padding
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"   // Hover effect
        "}"
        "QPushButton:pressed {"
        "    background-color: #1abc9c;"   // Pressed effect
        "}"
        );
    ui->lineEdit->setStyleSheet(
        "QLineEdit {"
        "    color: black;"                // Text color
        "}"
        );


    connect(ui->actionDelete_a_file, &QAction::triggered, this, &MainWindow::showPrompt);
    connect(ui->actioncontinue, &QAction::triggered, this, [=]() { OpenAddEdWindow(0); });
    connect(ui->actioncontinue_2, &QAction::triggered, this, [=]() { OpenAddEdWindow(1); });
    connect(ui->actionrefresh_table_menu, &QAction::triggered, this, &MainWindow::refTable);
    connect(ui->actionInit_Vdisk, &QAction::triggered, this, &MainWindow::InitPrompt);
    connect(ui->actionCompactage, &QAction::triggered, this, [=]() { compact_disk(filename); });

    refTable();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    QString GetTxt = ui->lineEdit->text();

    if (!GetTxt.isEmpty()) {
        ui->tableWidget->clear();
        FileMetaData *Search = (FileMetaData *)malloc(num_blocks * sizeof(FileMetaData));
        FileMetaData *Tmp = Search;
        TbCount = 0;

        for (int i = 0; i < num_blocks; ++i) {
            if (QString::fromUtf8(file_table[i].file_name).contains(GetTxt) ||
                GetTxt == QString::fromUtf8(file_table[i].file_name)) {
                strcpy(Tmp->file_name, (*(file_table + i)).file_name);
                Tmp->block_count = (*(file_table + i)).block_count;
                Tmp->first_block = (*(file_table + i)).first_block;
                Tmp->num_blocks = (*(file_table + i)).num_blocks;
                Tmp->org_mode_extern = (*(file_table + i)).org_mode_extern;
                Tmp->org_mode_intern = (*(file_table + i)).org_mode_intern;
                Tmp = Tmp + 1;
                TbCount++;
            }
        }

        ui->tableWidget->setRowCount(TbCount);
        ui->tableWidget->setColumnCount(3);
        ui->tableWidget->setHorizontalHeaderLabels({"File Name", "File Size", "First Block"});
        ui->tableWidget->verticalHeader()->setVisible(false);
        for (int row = 0; row < TbCount; ++row) {
            QTableWidgetItem *item = new QTableWidgetItem(Search[row].file_name);
            QTableWidgetItem *item1 = new QTableWidgetItem(
                QString::number(Search[row].block_count) + " records \\ " +
                QString::number(file_table[row].num_blocks) + " blocks");
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Search[row].first_block));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
            item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(row, 0, item);
            ui->tableWidget->setItem(row, 1, item1);
            ui->tableWidget->setItem(row, 2, item2);
            ui->tableWidget->setRowHeight(row, 80);
        }
    } else {
        refTable();
    }
}
