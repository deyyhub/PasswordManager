#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QCoreApplication>
#include <QSqlError>

void MainWindow::loadPasswords()
{
    QSqlQuery query(QSqlDatabase::database());

    query.exec("SELECT id, site, username, password FROM passwords");

    int row = 0;

    ui->passwordTable->setRowCount(0);

    while (query.next()) {

        int id = query.value(0).toInt();
        QString site = query.value(1).toString();
        QString user = query.value(2).toString();
        QString pass = query.value(3).toString();

        ui->passwordTable->insertRow(row);

        QTableWidgetItem *siteItem = new QTableWidgetItem(site);
        siteItem->setData(Qt::UserRole, id);

        ui->passwordTable->setItem(row, 0, siteItem);
        ui->passwordTable->setItem(row, 1, new QTableWidgetItem(user));
        ui->passwordTable->setItem(row, 2, new QTableWidgetItem(pass));

        row++;
    }
}

void MainWindow::onAddClicked(){


    int row = ui->passwordTable->rowCount();
    ui->passwordTable->insertRow(row);

    QString site = ui->siteInput->text();
    QString user = ui->userInput->text();
    QString pass = ui->passInput->text();

    QSqlQuery query;
    query.prepare("insert into passwords (site, username, password) VALUES (?,?,?)");

    query.addBindValue(site);
    query.addBindValue(user);
    query.addBindValue(pass);

    if (query.exec()) {
        ui->passwordTable->setItem(row, 0, new QTableWidgetItem(site));
        ui->passwordTable->setItem(row, 1, new QTableWidgetItem(user));
        ui->passwordTable->setItem(row, 2, new QTableWidgetItem(pass));
    }else {
        qDebug() << "Insert error:" << query.lastError().text();
    }


    ui->siteInput->clear();
    ui->userInput->clear();
    ui->passInput->clear();
}

void MainWindow::onDeleteClicked(){

    int row = ui->passwordTable->currentRow();

    if (row < 0)
        return;

    QTableWidgetItem *item = ui->passwordTable->item(row, 0);
    if (!item) return;

    int id = item->data(Qt::UserRole).toInt();

    QSqlQuery query(QSqlDatabase::database());
    query.prepare("DELETE FROM passwords WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Delete error:" << query.lastError().text();
        return;
    }

    ui->passwordTable->removeRow(row);
    ui->passwordTable->setCurrentCell(-1, -1);
}

void MainWindow::onSearchTextChanged(const QString &text){

    for(int row = 0; row<ui->passwordTable->rowCount(); row++){
        bool match = false;

        for(int col = 0; col < ui->passwordTable->columnCount(); col++){
            QTableWidgetItem *item = ui->passwordTable->item(row,col);

            if(item && item->text().contains(text,Qt::CaseInsensitive)){
                match = true;
                break;
            }
        }

        ui->passwordTable->setRowHidden(row,!match);
    }

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/vaultix.db");

    if (!db.open()) {
        qDebug() << "DB error:" << db.lastError().text();
    } else {
        qDebug() << "DB OK";
    }

    QSqlQuery query;
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS passwords (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            site TEXT,
            username TEXT,
            password TEXT
        )
    )");

    loadPasswords();

    connect(ui->addButton, &QPushButton::clicked,this,&MainWindow::onAddClicked);

    connect(ui->deleteButton, &QPushButton::clicked,this,&MainWindow::onDeleteClicked);

    connect(ui->SearchInput, &QLineEdit::textChanged,this,&MainWindow::onSearchTextChanged);

    ui->passwordTable->setColumnCount(3);

    ui->passwordTable->setHorizontalHeaderLabels(
        QStringList() << "Site" << "Username" << "Password"
        );
}

MainWindow::~MainWindow()
{
    delete ui;
}
