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
    connect(ui->membershipDueReport,
            &QPushButton::clicked,
            this,
            &MainWindow::membershipDueReport);
    connect(ui->membersUpgrade,
            &QPushButton::clicked,
            this,
            &MainWindow::onMembersUpgrade);
    connect(ui->membersDowngrade,
            &QPushButton::clicked,
            this,
            &MainWindow::onMembersDowngrade);
    connect(ui->itemUnitsSold,
            &QPushButton::clicked,
            this,
            &MainWindow::onItemsUnitsSold);

    connect(ui->memberManagementButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onMemberManagementButton);

    connect(ui->salesReportButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onSalesReportButton);

    for(int i = 1; i < ui->PageManager->count(); i++){
        QWidget* page = ui->PageManager->widget(i);

        QVector<QPushButton*> buttons = page->findChildren<QPushButton*>();
        for(auto button : buttons){
            QString buttonName = button->objectName();
            if(buttonName.contains("homeButton")){
                connect(button,
                        &QPushButton::clicked,
                        this,
                        &MainWindow::onHome);
            }
        }
    }

    QFont monospacedFont("Courier New");
    monospacedFont.setStyleHint(QFont::Monospace);
    monospacedFont.setPixelSize(12);
    ui->reportWindow->setFont(monospacedFont);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePage(int index) {
    ui->PageManager->setCurrentIndex(index);
}

void MainWindow::onHome() {
    changePage(0);
}

void MainWindow::onMemberManagementButton(){
    changePage(1);
}

void MainWindow::onSalesReportButton(){
    changePage(2);
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
    QVector<Member> members = memberManager.getAllMembers();
    QString list;

    for (const Member &m : members) {
        list += m.toString();
    }

    // QMessageBox::information(this,
    //                          "All Members",
    //                          list.isEmpty() ? "No members found." : list);

    setReportText(list);
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
    // memberManager.displayRebates(this);

    setReportText(memberManager.getRebates());
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

    QString report;
    if(day.isEmpty() || month.isEmpty()){
        report = memberManager.generateYearReport(year.toInt());
    }else{
        int d = day.toInt();
        int m = month.toInt();
        int y = year.toInt();
        Date date(d, m, y);

        std::cout<<date.toString().toStdString()<<std::endl;

        report = memberManager.generateDailyReport(date);
    }
    ui->reportWindow->setPlainText(report);
}


void MainWindow::onItemsSoldReport(){
    QVector<QPair<Item, int>> sortedVector;

    for(auto it = Purchase::itemList.constBegin(); it != Purchase::itemList.constEnd(); ++it){
        sortedVector.push_back(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedVector.begin(), sortedVector.end(), [](const QPair<Item, int> &a, const QPair<Item, int> &b){
        return a.second > b.second;
    });

    QString report = "";

    for(const auto& pair : sortedVector){
        report += QString("%1 %2 %3 \n\n")
                   .arg(pair.first.name, -20)
                   .arg(pair.second, 10)
                   .arg(pair.first.price * pair.second, 10);
    }
    setReportText(report);
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
    QString report = memberManager.generateTotalPurchaseReport();
    setReportText(report);
}


void MainWindow::membershipDueReport(){
    QString report = memberManager.generateYearlyDuesReport();
    setReportText(report);
}


void MainWindow::onMembersUpgrade() {
    QVector<Member> upgradeMembers = memberManager.getMembersShouldUpgrade();

    QString report = "";

    for(auto member: upgradeMembers){
        report += QString("%1 Possible Savings: %2\n")
                      .arg(member.getName())
                      .arg(member.getSavings());
    }

    if(upgradeMembers.isEmpty()){
        report = "None";
    }

    setReportText(report);
}

void MainWindow::onMembersDowngrade() {
    QVector<Member> downgradeMembers = memberManager.getMembersShouldDowngrade();

    QString report = "";

    for(auto member: downgradeMembers){
        report += QString("%1 Possible Savings: %3\n")
                      .arg(member.getName())
                      .arg(-1 * member.getSavings());
    }

    if(downgradeMembers.isEmpty()){
        report = "None";
    }
    setReportText(report);
}

void MainWindow::onItemsUnitsSold() {
    QString itemName = ui->itemNameInput->text();

    QString report = "";

    for(auto item : Purchase::itemList.keys()){
        if(item.name == itemName){
            report += QString("Item: %1\n").arg(itemName);
            report += QString("Units Sold: %1\n").arg(Purchase::itemList[item]);
            report += QString("Total Revenue: %1\n").arg(Purchase::itemList[item] * item.price);
        }
    }

    if(report.isEmpty()){
        QMessageBox::warning(this,
                             "Error", "Item Does Not Exist");
    }
    setReportText(report);
}

void MainWindow::setReportText(const QString& report){
    ui->reportWindow->setPlainText(report);
}













