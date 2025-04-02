#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 🔹 Load warehouse shoppers and daily sales
    memberManager.loadShoppersFile(":/data/static/warehouse shoppers.txt");
    // memberManager.processSalesFile(":/data/static/day1.txt");

    // 🔹 Ensure both buttons are properly connected
    connect(ui->addButton,
            &QPushButton::clicked,
            this,
            &MainWindow::addMember);
    connect(ui->removeButton,
            &QPushButton::clicked,
            this,
            &MainWindow::removeMember);
    connect(ui->searchButton,
            &QPushButton::clicked,
            this,
            &MainWindow::searchMember);
    connect(ui->displayButton,
            &QPushButton::clicked,
            this,
            &MainWindow::displayAllMembers);
    connect(ui->updateButton,
            &QPushButton::clicked,
            this,
            &MainWindow::updateMember);
    connect(ui->rebateButton,
            &QPushButton::clicked,
            this,
            &MainWindow::calculateRebates);
    connect(ui->loadSalesFile,
            &QPushButton::clicked,
            this,
            &MainWindow::onUploadFileClicked);
    connect(ui->generateReport,
            &QPushButton::clicked,
            this,
            &MainWindow::generateReport);
    connect(ui->itemsSoldReport,
            &QPushButton::clicked,
            this,
            &MainWindow::onItemsSoldReport);
    connect(ui->allPurchaseReport,
            &QPushButton::clicked,
            this,
            &MainWindow::onAllPurchaseReport);
    connect(ui->totalPurchaseReport,
            &QPushButton::clicked,
            this,
            &MainWindow::onTotalPurchaseReport);
}
MainWindow::~MainWindow()
{
    delete ui;
}

// 🔹 Add a new member
void MainWindow::addMember()
{
    QString name = ui->nameInput->text();
    int id = ui->idInput->text().toInt();
    QString expiry = ui->expiryInput->text();
    bool isPreferred = ui->typeInput->isChecked();
    Member::MembershipType type =
            isPreferred ? Member::PREFERRED : Member::BASIC;

    if (name.isEmpty() || id <= 0 || expiry.isEmpty()) {
        QMessageBox::warning(this,
                             "Error", "Please enter valid member details.");
        return;
    }

    memberManager.addMember(Member(name, id, type, expiry));
    QMessageBox::information(this, "Success", "Member added successfully!");
}

// 🔹 Remove an existing member
void MainWindow::removeMember()
{
    int id = ui->idInput->text().toInt();
    if (memberManager.removeMember(id)) {
        QMessageBox::information(this, "Success", "Member removed.");
    } else {
        QMessageBox::warning(this, "Error", "Member not found.");
    }
}

// 🔹 Search for a member by ID
void MainWindow::searchMember()
{
    int id = ui->idInput->text().toInt();
    Member *m = memberManager.searchMember(id);

    if (m) {
        QMessageBox::information(this, "Member Found", m->toString());
    } else {
        QMessageBox::warning(this, "Error", "Member not found.");
    }
}

// 🔹 Display all members
void MainWindow::displayAllMembers()
{
    QList<Member> members = memberManager.getAllMembers();
    QString list;

    for (const Member &m : members) {
        list += m.toString() + "\n";
    }

    QMessageBox::information(this,
                             "All Members",
                             list.isEmpty() ? "No members found." : list);
}

// 🔹 Update an existing member
void MainWindow::updateMember()
{
    int id = ui->idInput->text().toInt();
    Member *m = memberManager.searchMember(id);

    if (!m) {
        QMessageBox::warning(this, "Error", "Member not found!");
        return;
    }

    // Get updated values
    QString newName = ui->nameInput->text();
    QString newExpiry = ui->expiryInput->text();
    bool isPreferred = ui->typeInput->isChecked();
    Member::MembershipType newType =
            isPreferred ? Member::PREFERRED : Member::BASIC;

    if (newName.isEmpty() || newExpiry.isEmpty()) {
        QMessageBox::warning(this,
                             "Error", "Please enter valid member details.");
        return;
    }

    // Apply updates
    m->setName(newName);
    m->setExpiryDate(newExpiry);
    m->setType(newType);

    memberManager.saveToFile("members.txt");

    QMessageBox::information(this, "Success", "Member updated successfully!");
}

// 🔹 Calculate rebates for Preferred Members
void MainWindow::calculateRebates()
{
    memberManager.calculateRebates();
    memberManager.displayRebates(this);
}

void MainWindow::onUploadFileClicked() {
    QString fileName = QFileDialog::getOpenFileName(
            this,
            "Select Sales File",
            QDir::homePath(),
            "Text Files (*.txt);"
        );

    if(fileName.isEmpty()) return;

    memberManager.processSalesFile(fileName);
}

void MainWindow::generateReport(){
    QString day = ui->reportDayInput->text();
    QString month = ui->reportMonthInput->text();
    QString year = ui->reportYearInput->text();

    if(day.isEmpty() || month.isEmpty()){
        memberManager.generateYearReport(year.toInt());
    }else{
        int d = day.toInt();
        int m = month.toInt();
        int y = year.toInt();
        Date date(d, m, y);

        std::cout<<date.toString().toStdString()<<std::endl;

        memberManager.generateDailyReport(date);
    }
}


void MainWindow::onItemsSoldReport(){
    QVector<QPair<Item, int>> sortedVector;

    for(auto it = Purchase::itemList.constBegin(); it != Purchase::itemList.constEnd(); ++it){
        sortedVector.push_back(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedVector.begin(), sortedVector.end(), [](const QPair<Item, int> &a, const QPair<Item, int> &b){
        return a.second > b.second;
    });

    QString projectRoot = QCoreApplication::applicationDirPath() + "/..";
    QDir rootDir(projectRoot);
    rootDir.cdUp();
    rootDir.cdUp();

    QDir reportsDir(rootDir.filePath("reports"));
    if(!reportsDir.exists()){
        reportsDir.mkpath(".");
    }

    QString filename = "SoldItemsReport.txt";
    QString fullPath = reportsDir.filePath(filename);

    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);

    for(const auto& pair : sortedVector){
        out<<QString("%1 %2 %3 \n\n")
                   .arg(pair.first.name, -20)
                   .arg(pair.second, 10)
                   .arg(pair.first.price * pair.second, 10);
    }
}


void MainWindow::onAllPurchaseReport(){
    double totalAmount = 0;
    int id = ui->idInput->text().toInt();

    Member *m = memberManager.searchMember(id);

    if (!m) {
        QMessageBox::warning(this, "Error", "Member not found!");
        return;
    }

    QVector<Purchase> allPurchases = m->getAllPurchases();


    QString projectRoot = QCoreApplication::applicationDirPath() + "/..";
    QDir rootDir(projectRoot);
    rootDir.cdUp();
    rootDir.cdUp();

    QDir reportsDir(rootDir.filePath("reports"));
    if(!reportsDir.exists()){
        reportsDir.mkpath(".");
    }

    QString filename = m->getName() + "-AllPurchaseReport.txt";
    QString fullPath = reportsDir.filePath(filename);

    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);


    for(auto purchase : allPurchases){
        totalAmount += purchase.getTotalPrice();

        out<<QString("%1 %2 %3 \n\n")
                   .arg(purchase.getItem().name, -25)
                   .arg(purchase.getQuantity(), 10)
                   .arg(purchase.getTotalPrice(), 10);
    }

    out<<QString("Total Amount: %1 \n").arg(totalAmount);
}


void MainWindow::onTotalPurchaseReport(){
    memberManager.generateTotalPurchaseReport();
}
















