#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 🔹 Load warehouse shoppers and daily sales
    // memberManager.loadShoppersFile(":/data/static/warehouse shoppers.txt");

    bool isDataRestored = restoreData();
    std::cout<<isDataRestored<<std::endl;

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
    // connect(ui->updateButton,
    //         &QPushButton::clicked,
    //         this,
    //         &MainWindow::updateMember);
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

    connect(ui->expiringMembers,
            &QPushButton::clicked,
            this,
            &MainWindow::onGetExpiringMembers);

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
void MainWindow::addMember() {

    QString name = ui->nameInput->text();
    int id = ui->idInput->text().toInt();
    QString expiry = ui->expiryInput->text();
    bool isPreferred = ui->typeInput->isChecked();
    Member::MembershipType type =
            isPreferred ? Member::PREFERRED : Member::BASIC;


    QDate expiryDate = QDate::fromString(expiry, "MM/dd/yyyy");

    if(!expiryDate.isValid()
        || name.isEmpty() || id <= 0 || memberManager.containsMember(id)){

        QMessageBox::warning(this,
                             "Error", "Please enter valid member details.");
        return;
    }

    Member member(name, id, type, expiryDate);
    memberManager.addMember(member);

    addMemberToMembersFile(member);

    QMessageBox::information(this, "Success", "Member added successfully!");
}

// 🔹 Remove an existing member
void MainWindow::removeMember()
{
    int id = ui->idInput->text().toInt();

    if (memberManager.removeMember(id)) {
        QMessageBox::information(this, "Success", "Member removed.");

        removeIdFromMembersFile(id);
    } else {
        QMessageBox::warning(this, "Error", "Member not found.");
    }
}

// 🔹 Search for a member by ID
void MainWindow::searchMember()
{
    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();

    int id = ui->idInput->text().toInt();
    QString name = ui->nameInput->text();

    Member *m = memberManager.searchMember(id, includedTypes);

    if(!m){
        m = memberManager.searchMember(name, includedTypes);
        if(!m){
            QMessageBox::warning(this, "Error", "Member not found.");
            return;
        }
    }

    QString report = QString(
                         "Name: %1\n"
                         "ID: %2\n"
                         "Membership Type: %3\n"
                         "Expiration Date: %4\n"
                         "Total Spent: $%5\n"
                         "Rebate Amount: $%6"
                         ).arg(m->getName())
                         .arg(m->getId())
                         .arg(m->isPreferred() ? "Preferred" : "Basic")
                         .arg(m->getExpiryDate().toString("MM/dd/yyyy"))
                         .arg(m->getTotalSpent())
                         .arg(m->getRebate());

    setReportText(report);
    QMessageBox::information(this, "Member Found", "Member Found");
}

// 🔹 Display all members
void MainWindow::displayAllMembers()
{

    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();

    QVector<Member> members = memberManager.getAllMembers(includedTypes);
    QString list;

    list += QString("%1%2%3%4%5%6\n\n")
                .arg("Name", -25)
                .arg("ID", -10)
                .arg("Type", -12)
                .arg("Expiry Date", -12)
                .arg("Spent", -12)
                .arg("Rebate", -10);

    for (const Member &m : members) {
        list += m.toString();
    }

    setReportText(list);
}

// 🔹 Update an existing member
// void MainWindow::updateMember()
// {
//     int id = ui->idInput->text().toInt();
//     Member *m = memberManager.searchMember(id);

//     if (!m) {
//         QMessageBox::warning(this, "Error", "Member not found!");
//         return;
//     }

//     // Get updated values
//     QString newName = ui->nameInput->text();
//     QString newExpiry = ui->expiryInput->text();
//     bool isPreferred = ui->typeInput->isChecked();
//     Member::MembershipType newType =
//             isPreferred ? Member::PREFERRED : Member::BASIC;

//     if (newName.isEmpty() || newExpiry.isEmpty()) {
//         QMessageBox::warning(this,
//                              "Error", "Please enter valid member details.");
//         return;
//     }

//     // Apply updates
//     m->setName(newName);
//     m->setExpiryDate(QDate::fromString(newExpiry, "MM/dd/yyyy"));
//     m->setType(newType);

//     memberManager.saveToFile("members.txt");

//     QMessageBox::information(this, "Success", "Member updated successfully!");
// }

// 🔹 Calculate rebates for Preferred Members
void MainWindow::calculateRebates()
{
    memberManager.calculateRebates();
    setReportText(memberManager.getRebates());
}


void MainWindow::onUploadFileClicked() {
    QDir path = QDir(QCoreApplication::applicationDirPath());

    path.cdUp();
    path.cdUp();
    path.cdUp();


    QString fileName = QFileDialog::getOpenFileName(
            this,
            "Select Sales File",
            path.absolutePath(),
            "Text Files (*.txt);"
        );

    if(fileName.isEmpty()) return;

    if(MemberManager::processedFiles.contains(fileName)){
        return;
    }

    std::cout<<fileName.toStdString()<<std::endl;

    memberManager.processSalesFile(fileName);

    MemberManager::processedFiles.insert(fileName);

    QString homeDir = GET_PROJECT_DIRECTORY();
    QString purchaseFilesProcessedDir = homeDir + "/db/purchaseFilesProcessed.txt";

    std::cout<<purchaseFilesProcessedDir.toStdString()<<std::endl;
    std::cout<<"Here"<<std::endl;
    QFile purchaseFilesProcessed(purchaseFilesProcessedDir);
    std::cout<<"Here"<<std::endl;

    if (purchaseFilesProcessed.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&purchaseFilesProcessed);
        out << fileName << "\n";
        purchaseFilesProcessed.close();
        qDebug() << "Data appended successfully.";
    } else {
        qWarning() << "Failed to open file for appending.";
    }
}

void MainWindow::generateReport(){
    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();

    QString day = ui->reportDayInput->text();
    QString month = ui->reportMonthInput->text();
    QString year = ui->reportYearInput->text();

    QString report;

    if(day.isEmpty() || month.isEmpty()){
        if(year.isEmpty() || year.toInt() <= 0){
            QMessageBox::warning(this,
                                 "Error", "Please enter valid date or year.");
            return;
        }
        report = memberManager.generateYearReport(year.toInt(), includedTypes);
    }else{
        int d = day.toInt();
        int m = month.toInt();
        int y = year.toInt();
        QDate date(y, m, d);

        if(!date.isValid()){
            QMessageBox::warning(this,
                                 "Error", "Please enter valid date or year.");
            return;
        }
        report = memberManager.generateDailyReport(date, includedTypes);
    }
    setReportText(report);
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
    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();
    double totalAmount = 0;

    QString searchType = QInputDialog::getItem(
        this,
        "Search By",
        "Choose how you want to search:",
        {"Name", "ID"},  // options
        0,               // default index
        false            // editable? (false = dropdown only)
        );


    Member* m;
    if (!searchType.isEmpty()) {
        if (searchType == "Name") {
            bool ok;
            QString name = QInputDialog::getText(this, "Search by Name", "Enter name:", QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                m = memberManager.searchMember(name, includedTypes);
            }
        } else if (searchType == "ID") {
            bool ok;
            QString id = QInputDialog::getText(this, "Search by ID", "Enter ID:", QLineEdit::Normal, "", &ok);
            if (ok && !id.isEmpty()) {
                m = memberManager.searchMember(id.toInt(), includedTypes);
            }
        }
    }


    if (!m) {
        QMessageBox::warning(this, "Error", "Member not found!");
        return;
    }

    QVector<Purchase> allPurchases = m->getAllPurchases();

    QString report = QString("All Purchase Report for: %1\n\n")
                        .arg(m->getName());
    for(auto purchase : allPurchases){
        totalAmount += purchase.getTotalPrice();

        report += QString("%1 %2 %3 \n\n")
                   .arg(purchase.getItem().name, -25)
                   .arg(purchase.getQuantity(), 10)
                   .arg(purchase.getTotalPrice(), 10);
    }

    report += QString("Total Amount: %1 \n").arg(totalAmount);

    setReportText(report);
}


void MainWindow::onTotalPurchaseReport(){
    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();
    QString report = memberManager.generateTotalPurchaseReport(includedTypes);
    setReportText(report);
}


void MainWindow::membershipDueReport(){
    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();
    QString report = memberManager.generateYearlyDuesReport(includedTypes);
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

void MainWindow::onGetExpiringMembers() {
    bool ok = false;

    // Ask for month
    int month = QInputDialog::getInt(
        this, "Enter Month", "Enter expiration month (1–12):", QDate::currentDate().month(), 1, 12, 1, &ok);
    if (!ok) return;

    // Ask for year
    int year = QInputDialog::getInt(
        this, "Enter Year", "Enter expiration year (e.g., 2025):", QDate::currentDate().year(), 1900, 2100, 1, &ok);
    if (!ok) return;


    QSet<Member::MembershipType> includedTypes = getIncludedTypesSet();
    QVector<Member> expiringMembers = memberManager.getExpiringMembers(month, year, includedTypes);

    std::sort(expiringMembers.begin(),
              expiringMembers.end(),
              [] (const Member& a, const Member& b){
                  return a.getExpiryDate() < b.getExpiryDate();
              });

    QString report = "Expiring Members Report: \n\n";

    for(auto member: expiringMembers){
        report += QString("%1%2Due: $%3\n")
                      .arg(member.getName(), -30)
                      .arg(member.getExpiryDate().toString("MM/dd/yyyy"), -20)
                      .arg(member.getDues());
    }

    setReportText(report);
}


bool MainWindow::restoreData() {
    QFile defaultMembers(":/data/static/warehouse shoppers.txt");

    QString currentMembersDir = GET_MEMBERS_FILE_DIRECTORY();
    QString purchaseFilesProcessedDir = GET_PURCHASES_PROCESSED_DIRECTORY();

    // copyDefaultMembersToDb(defaultMembers);
    getDefaultMemberIfEmpty(defaultMembers);

    QFile purchaseFilesProcessed(purchaseFilesProcessedDir);
    QFile currentMembers(currentMembersDir);


    if(!currentMembers.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }
    memberManager.loadShoppersFileFormatted();


    if(!purchaseFilesProcessed.open(QIODevice::ReadOnly | QIODevice::Text)){
            std::cout<<"Problem"<<std::endl;
        return false;
    }

    QTextStream in(&purchaseFilesProcessed);

    while(!in.atEnd()){
        QString filePath = in.readLine().trimmed();

        if(filePath.isEmpty()) continue;

        memberManager.processSalesFile(filePath);
    }

    return true;
}


void MainWindow::setReportText(const QString& report){
    ui->reportWindow->setPlainText(report);
}


void MainWindow::getDefaultMemberIfEmpty(QFile& defaultFile){
    QString membersFileDir = GET_MEMBERS_FILE_DIRECTORY();
    QFile currentFile(membersFileDir);

    if (currentFile.exists() && currentFile.size() > 0) {
        qDebug() << "Current members file already exists and is not empty. Skipping initialization.";
        return;
    }

    if(!defaultFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Failed to open default members file:" << defaultFile.errorString();
        return;
    }

    if (!currentFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open current members file for writing:" << currentFile.errorString();
        defaultFile.close();
        return;
    }

    QTextStream in(&defaultFile);
    QTextStream out(&currentFile);

    while(!in.atEnd()){
        QString name = in.readLine().trimmed();
        QString id = in.readLine().trimmed();
        QString type = in.readLine().trimmed();
        QString expiration = in.readLine().trimmed();

        if(name.isEmpty() || id.isEmpty() || type.isEmpty() || expiration.isEmpty()){
            qDebug() << "Invalid entry found, skipping.";
            continue;
        }

        QString formatted = QString("%1, %2, %3, %4")
                                .arg(id)
                                .arg(name)
                                .arg(type)
                                .arg(expiration);
        out << formatted << "\n";
    }

    defaultFile.close();
    currentFile.close();
    qDebug() << "Current members file initialized successfully.";
}


QSet<Member::MembershipType> MainWindow::getIncludedTypesSet(){
    QSet<Member::MembershipType> includedTypes;

    if(ui->preferredCheckbox->isChecked()){
        includedTypes.insert(Member::PREFERRED);
    }

    if(ui->basicCheckbox->isChecked()){
        includedTypes.insert(Member::BASIC);
    }

    return includedTypes;
}












